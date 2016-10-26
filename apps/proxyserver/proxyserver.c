/*
 *  Copyright 2013 People Power Company
 *  
 *  This code was developed with funding from People Power Company
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 * This is a stand-alone proxy server application.  It accepts socket connections
 * which communicates messages bi-directionally with the cloud server.
 *
 * @author Andrey Malashenko
 * @author David Moss
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdbool.h>

#include <curl/curl.h>
#include <libxml/parser.h>

#include "libconfigio.h"
#include "libpipecomm.h"

#include "settings.h"
#include "login.h"
#include "user.h"
#include "getactivationinfo.h"
#include "proxyactivation.h"
#include "ioterror.h"
#include "iotdebug.h"
#include "proxy.h"
#include "proxyconfig.h"
#include "proxyserver.h"
#include "proxyclientmanager.h"
#include "proxyagent.h"
#include "proxycli.h"
#include "proxymanager.h"
#include "eui64.h"



/** Process termination flag */
static int gTerminate = false;
char *argEui64Bytes = NULL;
char *argDeviceType = NULL;

/** Client socket FD */
static int clientSocketFd;

/***************** Prototypes ***************/
void _proxyserver_processMessage(int clientSocketFd);

void _proxyserver_listener(const char *message, int len);

void _timer_handler ( int signum );

void api_update_timer_init (void);

void timer_thread_init ( void );


static char eui64[EUI64_STRING_SIZE+8];
pthread_t timer_thread;
/***************** Functions *****************/
/**
 * Main function
 */
int main(int argc, char *argv[]) {
  int sockfd;
  int pid;
  socklen_t clientLen;
  struct sockaddr_in serverAddress;
  struct sockaddr_in clientAddress;

  // Ignore the SIGCHLD signal to get rid of zombies
  signal(SIGCHLD, SIG_IGN);

  // Don't crash when we write to a broken pipe
  signal(SIGPIPE, SIG_IGN);

  // Parse the command line arguments
  proxycli_parse(argc, argv);

  printf("Using configuration file %s\n", proxycli_getConfigFilename());
  SYSLOG_INFO("Using configuration file %s", proxycli_getConfigFilename());

  // Get Presto connection settings
  eui64_toString(eui64, sizeof(eui64));
  printf("The proxy device ID is %s\n", eui64);
  SYSLOG_INFO("The proxy device ID is %s\n", eui64);

  if ( proxycli_getCloudName() == NULL )
  {
      char cloudName[PATH_MAX];
      if( libconfigio_read(proxycli_getConfigFilename(), CONFIGIO_CLOUD_NAME, cloudName, sizeof(cloudName)) > -1 )
      {
	  proxycli_setCloudName(cloudName);
      }
      else
      {
	  printf("Couldn't read %s in file %s, writing default value\n", CONFIGIO_CLOUD_NAME, proxycli_getConfigFilename());
	  libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_CLOUD_NAME, DEFAULT_CLOUD_NAME);
	  proxycli_setCloudName(DEFAULT_CLOUD_NAME);
      }
  }
  else
  {
      libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_CLOUD_NAME, proxycli_getCloudName());
  }

  if ( proxycli_getDataFormat() == NULL )
  {
      char dataFormat[PATH_MAX];
      if( libconfigio_read(proxycli_getConfigFilename(), CONFIGIO_DATA_FORMAT_TOKEN_NAME, dataFormat, sizeof(dataFormat)) > -1 )
      {
	  proxycli_setDataFormat(dataFormat);
      }
      else
      {
	  printf("Couldn't read %s in file %s, writing default value %s\n", CONFIGIO_DATA_FORMAT_TOKEN_NAME, proxycli_getConfigFilename(), DEFAULT_DATA_FORMAT);
	  libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_DATA_FORMAT_TOKEN_NAME, DEFAULT_DATA_FORMAT);
	  proxycli_setDataFormat(DEFAULT_DATA_FORMAT);
      }
  }
  else
  {
      libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_DATA_FORMAT_TOKEN_NAME, proxycli_getDataFormat());
  }

  getConnectionSettings(eui64, proxycli_getCloudName());

  // If the CLI tells us to activate this proxy, then activate it and exit now.
  if(proxycli_readyToActivate()) {
    if(proxycli_getActivationKey() != NULL) {
      // Activate with an activation key
      if(proxyactivation_activate(proxycli_getActivationKey(), NULL) == SUCCESS) {
        printf("Activated! Exiting.\n\n");
        SYSLOG_INFO("Activated! Exiting.\n\n");
        exit(0);

      }

      printf("Activation failed. Exiting. (proxycli_getActivationKey)\n\n");
      SYSLOG_ERR("Activation failed. Exiting. (proxycli_getActivationKey)\n\n");
      exit(1);

    } else {
      // Register device with username and password
      if(login_doLogin(proxycli_getUsername(), proxycli_getPassword()) == SUCCESS) {
	  // Activate with username and password
#ifdef PROXY_ACTIVATION
	  if(proxyactivation_activate(getactivationinfo_getDeviceActivationKey(login_getApiKey(), user_getLocationId(login_getApiKey())), proxycli_getUsername()) == SUCCESS) {
	      printf("Activated! Exiting.\n\n");
	      SYSLOG_INFO("Activated! Exiting.\n\n");
	      exit(0);
	  }
#endif
	  if ( registerDevice() == 0 )
	  {
	      printf("Proxy registered!\n\n");
	      SYSLOG_INFO("Proxy registered!\n\n");
	  }
	  else
	  {
	      printf("Activation failed. Exiting. (login_doLogin) \n\n");
	      SYSLOG_ERR("Activation failed. Exiting. (login_doLogin)\n\n");
	      exit(1);
	  }
      }

    }
  }

  // Start up the proxy
  proxymanager_startProxy();

  // Start our proxy agent to take commands from the server to control the proxy
  if(proxyagent_start() != SUCCESS) {
    exit(1);
  }

  // Add a listener to the proxy so we can forward commands from the server to other clients / agents
  if (proxylisteners_addListener(&_proxyserver_listener) != SUCCESS) {
    SYSLOG_ERR("[%d]: Proxy is out of listener slots", getpid());
    exit(1);
  }

  // Setup a socket so external clients can connect to this proxy server
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    SYSLOG_ERR("ERROR opening socket");
  }

  bzero((char *) &serverAddress, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(proxycli_getPort());

  if (bind(sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
    SYSLOG_ERR("ERROR on binding");
    printf("Could not bind to port %d\n", proxycli_getPort());
    exit(1);
  }

  listen(sockfd, 5);
  clientLen = sizeof(clientAddress);

  // Finally, the following loop accepts new client socket connections
  SYSLOG_INFO("Proxy running; port=%d; pid=%d\n", proxycli_getPort(), getpid());
  printf("Proxy running; port=%d; pid=%d\n", proxycli_getPort(), getpid());

  // Initial timer thread.
  timer_thread_init();

  while (!gTerminate) {
    clientSocketFd = accept(sockfd, (struct sockaddr *) &clientAddress, &clientLen);

    if (clientSocketFd < 0) {
      SYSLOG_ERR("ERROR on accept");
      continue;
    }

    if (proxyclientmanager_add(clientSocketFd) != SUCCESS) {
      SYSLOG_ERR("[%d]: Out of client elements to track sockets", getpid());
      close(clientSocketFd);

    } else {
      pid = fork();
      if (pid < 0) {
        SYSLOG_ERR("ERROR on fork");

      } else if (pid == 0) {
        // Child process
        SYSLOG_INFO("[%d]: New client listener created", getpid());
        close(sockfd);


	while (true) {
          // Read messages from the client until the socket is closed
          _proxyserver_processMessage(clientSocketFd);
          sleep(1);
        }

        proxyclientmanager_remove(clientSocketFd);
        SYSLOG_INFO("[%d]: Destroyed", getpid());
        exit(0);

      } else {
	  // Parent process

      }
    }
  }

  SYSLOG_INFO("*************** SHUTTING DOWN PROXY ***************");
  printf("Done!\n");

  xmlCleanupParser();
  xmlMemoryDump();

  close(sockfd);
  pthread_exit(NULL);
  return 0;
}


/**
 * Registered listener to the proxy. This broadcasts received server messages
 * to all client sockets
 *
 * @param message Message from the server
 * @param len Length of the message from the server
 */
void _proxyserver_listener(const char *message, int len) {
  int i;
  int clients = 0;
  proxy_client_t *client;

  for(i = 0; i < proxyclientmanager_size(); i++) {
    client = proxyclientmanager_get(i);
    if(client->inUse) {
      if (libpipecomm_write(client->fd, message, len) < 0) {
        SYSLOG_ERR("ERROR writing to socket %d, closing socket", client->fd);
        proxyclientmanager_remove(client->fd);
        close(client->fd);

      } else {
        clients++;
      }
    }
  }

  SYSLOG_DEBUG("Broadcast message to %d sockets", clients);
}

/**
 * Receives message from socket and passes it to the pipe.  The message will
 * be picked up by another thread and sent to the server
 *
 * @param clientSocketFd The client socket file descriptor
 */
void  _proxyserver_processMessage(int clientSocketFd) {
  int n;
  char buffer[PROXY_MAX_MSG_LEN];
  bzero(buffer, PROXY_MAX_MSG_LEN);

  if ((n = read(clientSocketFd, buffer, PROXY_MAX_MSG_LEN)) > 0) {
    proxy_send(buffer, n);

  } else if(n == 0) {
    SYSLOG_ERR("[%d]: Socket killed, destroying thread", getpid());
    proxyclientmanager_remove(clientSocketFd);
    close(clientSocketFd);
    exit(0);

  } else {
    SYSLOG_ERR("[%d]: Error reading from socket, destroying thread", getpid());
    proxyclientmanager_remove(clientSocketFd);
    close(clientSocketFd);
    exit(0);
  }

}

/**
 * Application API update timer initiator
 * 
 */
void api_update_timer_init (void)
{
    char updatePeriod[16];
    struct sigaction sa;
    struct itimerval timer;
    
    /* Install timer_handler as the signal handler for SIGVTALRM. */
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &_timer_handler;
    sigaction(SIGVTALRM, &sa, NULL);

    /* Configure */
    if(libconfigio_read(proxycli_getConfigFilename(), CONFIGIO_API_URL_UPDATE_PERIOD, updatePeriod, sizeof(updatePeriod)) == -1) {
	printf("Couldn't read %s in file %s, writing default value\n", CONFIGIO_API_URL_UPDATE_PERIOD, proxycli_getConfigFilename());
	libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_API_URL_UPDATE_PERIOD, DEFAULT_API_UPDATE_PERIOD);
	strncpy(updatePeriod, DEFAULT_API_UPDATE_PERIOD, sizeof(updatePeriod));
    }

    timer.it_value.tv_sec = atoi(updatePeriod);
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = atoi(updatePeriod);
    timer.it_interval.tv_usec = 0;
    
    if ( setitimer(ITIMER_VIRTUAL, &timer, NULL) == 0 )
    {
	while (1);
    }
    else
    {
	SYSLOG_ERR("Get Presto connection setting interval initiate failed.\n");
    }
}

/**
 * Timer thread
 * 
 */
void *timer_thread_func ( void *arg )
{
    // Inital a timer for update application API url from APP API: Available Presto instances
    api_update_timer_init();

    return NULL;
}

/**
 * Timer thread initiator
 * 
 */
void timer_thread_init ( void )
{
    pthread_create(&timer_thread, NULL, timer_thread_func, NULL);
}

/**
 * Application API update timer handler
 *
 * @param signum
 */
void _timer_handler ( int signum )
{
    getConnectionSettings(eui64, proxycli_getCloudName());
}

