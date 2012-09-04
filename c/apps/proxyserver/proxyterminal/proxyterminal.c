/*
 * Copyright (c) 2010 People Power Company
 * All rights reserved.
 *
 * This open source code was developed with funding from People Power Company
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the People Power Corporation nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * PEOPLE POWER CO. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE
 */

/**
 * This module opens an additional local port for the terminal on the local
 * network, and lets other clients connect into this port for the exclusive
 * purpose of managing the configuration and operation of the proxyserver.
 *
 * Commands you can send to the proxyserver:
 *
 *  "send" - Causes the proxyserver to send its buffer of measurements now
 *
 *
 * The terminal will respond with the following:
 *  "OK" - The command executed successfully
 *  "FAIL : [reason]" - The command did not execute
 *
 * @author David Moss
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>

#include "libpipecomm.h"

#include "ioterror.h"
#include "iotdebug.h"
#include "proxyserver.h"
#include "proxyterminal.h"
#include "terminalcommandprocessor.h"
#include "iotapi.h"

/** Thread termination flag */
static bool gTerminate;

/** Thread */
static pthread_t sThreadId;

/** Thread attributes */
static pthread_attr_t sThreadAttr;

/** Client socket FD */
static int clientSocketFd;


/***************** Private Prototypes ****************/
static void *_terminalThread(void *params);

void _proxyterminal_respond(const char *message, int len, int socketFd);

void  _proxyterminal_processMessage(int clientSocketFd);


/***************** Public Functions ****************/
error_t proxyterminal_start() {

  // Initialize the thread
  pthread_attr_init(&sThreadAttr);

  // Will run detached from the main dispatcher thread
  pthread_attr_setdetachstate(&sThreadAttr, PTHREAD_CREATE_DETACHED);

  // Round robin schedule is fine when processing is extremely low
  pthread_attr_setschedpolicy(&sThreadAttr, SCHED_RR);

  // Create the thread
  if (pthread_create(&sThreadId, &sThreadAttr, &_terminalThread, NULL)) {
    SYSLOG_ERR("Creating terminal thread failed: %s", strerror(errno));
    proxyterminal_stop();
    return FAIL;
  }

  return SUCCESS;
}

/**
 * Close the thread
 * @return SUCCESS if the socket closed successfully, FAIL if it wasn't open
 */
void proxyterminal_stop() {
  gTerminate = true;
}

/**
 * Respond with a message to a specific socket file descriptor
 *
 * @param message Message from the server
 * @param len Length of the message from the server
 * @param socketFd the socket file descriptor to respond to
 */
void proxyterminal_respond(const char *message, int len, int socketFd) {
  if (libpipecomm_write(socketFd, message, len) < 0) {
    SYSLOG_ERR("ERROR writing to socket %d, closing socket", socketFd);
    close(socketFd);
    exit(0); // from the forked thread
  }
}

/***************** Private Functions ****************/
/**
 * Thread to read bytes from terminal port
 */
static void *_terminalThread(void *params) {
  int sockfd;
  int pid;
  socklen_t clientLen;
  struct sockaddr_in serverAddress;
  struct sockaddr_in clientAddress;

  // Setup a socket so external clients can connect to this terminal server
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    SYSLOG_ERR("ERROR opening socket");
  }

  bzero((char *) &serverAddress, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(DEFAULT_TERMINAL_PORT);

  if (bind(sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
    SYSLOG_ERR("[terminal] ERROR on binding");
    printf("[terminal] Could not bind to port\n");
    exit(1);
  }

  listen(sockfd, 5);
  clientLen = sizeof(clientAddress);

  // Finally, the following loop accepts new client connections
  SYSLOG_INFO("Local Command Terminal running; port=%d; pid=%d\n", DEFAULT_TERMINAL_PORT, getpid());
  printf("Local Command Terminal running; port=%d; pid=%d\n", DEFAULT_TERMINAL_PORT, getpid());

  while(!gTerminate) {
    clientSocketFd = accept(sockfd, (struct sockaddr *) &clientAddress, &clientLen);

    if (clientSocketFd < 0) {
      SYSLOG_ERR("ERROR on accept");
      continue;
    }


    pid = fork();
    if (pid < 0) {
      SYSLOG_ERR("ERROR on fork");

    } else if (pid == 0) {
      // Child process
      SYSLOG_INFO("[%d]: New client listener created", getpid());
      close(sockfd);
      _proxyterminal_processMessage(clientSocketFd);
      SYSLOG_INFO("[%d]: Destroying Terminal Client Thread", getpid());
      exit(0);

    } else {
      // Parent process

    }

  }

  SYSLOG_INFO("*** Exiting Terminal Thread ***");
  pthread_exit(NULL);
  return NULL;
}

/**
 * Receives message from socket and passes it to the pipe.  The message will
 * be picked up by another thread and sent to the server
 *
 * @param clientSocketFd The client socket file descriptor
 */
void  _proxyterminal_processMessage(int clientSocketFd) {
  int length;
  char message[TERMINAL_MAX_MSG_LEN];
  bzero(message, TERMINAL_MAX_MSG_LEN);

  if ((length = read(clientSocketFd, message, TERMINAL_MAX_MSG_LEN)) > 0) {
    terminalcommandprocessor_execute(message, length, clientSocketFd);

  } else if(length == 0) {
    SYSLOG_ERR("[%d]: Socket killed, destroying thread", getpid());
    close(clientSocketFd);
    exit(0);

  } else {
    SYSLOG_ERR("[%d]: Error reading from socket, destroying thread", getpid());
    close(clientSocketFd);
    exit(0);
  }

}

