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
 * Command line Interface for the proxy server
 * @author David Moss
 */

#include <linux/limits.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "proxycli.h"
#include "proxyserver.h"
#include "ioterror.h"
#include "iotdebug.h"


/** Port number */
static int port = DEFAULT_PROXY_PORT;

/** Application API base URL */
static char applicationApiBaseUrl[PATH_MAX];

/** Application API URL */
static char applicationApiUrl[PATH_MAX];

/** Pointer to the activation key string */
static char *activationKey;

/** Configuration file containing non-volatile proxy information */
static char configFilename[PROXYCLI_CONFIG_FILE_PATH_SIZE];

/** Activation username */
static char *username;

/** Activation password */
static char *password;

//** Data Format */
static char *dataFormat = NULL;

//** Cloud Name */
static char *cloudName = NULL;

/***************** Private Prototypes ****************/
static void _proxycli_printUsage();
static void _proxycli_printVersion();

/***************** Public Functions ****************/
/**
 * Parse the command line arguments, to be retrieved by getter functions when
 * needed
 */
void proxycli_parse(int argc, char *argv[]) {
  int c;

  strncpy(configFilename, DEFAULT_PROXY_CONFIG_FILENAME, PROXYCLI_CONFIG_FILE_PATH_SIZE);
  strncpy(applicationApiBaseUrl, DEFAULT_APPLICATION_API_BASE_URL, sizeof(applicationApiBaseUrl));

  while ((c = getopt(argc, argv, "b:u:p:c:l:n:a:f:v")) != -1) {
    switch (c) {
    case 'b':
      strncpy(applicationApiBaseUrl, optarg, sizeof(applicationApiBaseUrl));
      printf("[cli] The application API base URL is %s\n", applicationApiBaseUrl);
      SYSLOG_INFO("[cli] The application API base URL is %s", applicationApiBaseUrl);
      break;

    case 'c':
      strncpy(configFilename, optarg, PROXYCLI_CONFIG_FILE_PATH_SIZE);
      printf("[cli] Proxy config file set to %s\n", configFilename);
      SYSLOG_INFO("[cli] Proxy config file set to %s", configFilename);
      break;

    case 'l':
      cloudName = strdup(optarg);
      printf("[cli] Proxy cloud name set to %s\n", cloudName);
      SYSLOG_INFO("[cli] Proxy cloud name set to %s", cloudName);
      break;

    case 'f':
      if ( strcmp(optarg, "xml") == 0 || strcmp(optarg, "json") == 0 )
      {
	  dataFormat = strdup(optarg);
	  printf("[cli] Data format set to %s\n", dataFormat);
	  SYSLOG_INFO("[cli] Data format set to %s", dataFormat);
      }
      else
      {
	  printf("[cli] Invalid data format: %s\n", optarg);
	  SYSLOG_INFO("[cli] Invalid data format: %s", optarg);
      }
      break;

    case 'n':
      // Set the port number
      port = atoi(optarg);
      break;

    case 'a':
      // Register using the given activation code
      activationKey = optarg;
      printf("[cli] Activating with key %s\n", activationKey);
      SYSLOG_INFO("[cli] Activating with key %s", activationKey);
      break;

    case 'u':
      username = optarg;
      printf("[cli] Username set to %s\n", username);
      SYSLOG_INFO("[cli] Username set to %s\n", username);
      break;

    case 'p':
      password = optarg;
      printf("[cli] Password captured\n");
      SYSLOG_INFO("[cli] Password captured\n");
      break;

    case 'v':
      _proxycli_printVersion();
      exit(0);
      break;

    case '?':
      _proxycli_printUsage();
      exit(1);
      break;

    default:
      printf("[cli] Unknown argument character code 0%o\n", c);
      SYSLOG_ERR ("[cli] Unknown argument character code 0%o\n", c);
      _proxycli_printUsage();
      exit(1);
      break;
    }
  }
}

/**
 * @return the port number to open on
 */
int proxycli_getPort() {
  return port;
}

/**
 * @return the activation key, NULL if it was never set
 */
const char *proxycli_getActivationKey() {
  return activationKey;
}

/**
 * @return The configuration filename for the proxy
 */
const char *proxycli_getConfigFilename() {
  return configFilename;
}

/**
 * Set the cloud name which the proxy going to interaction with
 * @param name The cloud name
 * @return The cloud which the proxy interactiion with
 */
const char *proxycli_setCloudName( const char *name ) {
    if ( cloudName != NULL ) free(cloudName);
    cloudName = strdup(name);
    return cloudName;
}

/**
 * Set the data format proxy interaction with FabrUX
 * @param format The data format proxy going to interaction with FabrUX
 * @return The data format which the proxy interactiion with FabrUX
 */
const char *proxycli_setDataFormat ( const char *format ) {
    if ( dataFormat != NULL ) free(dataFormat);
    dataFormat = strdup(format);
    return dataFormat;
}

/**
 * @return The cloud which the proxy interactiion with
 */
const char *proxycli_getCloudName ( void ) {
  return cloudName;
}

/**
 * @return The data format which the proxy interactiion with FabrUX
 */
const char *proxycli_getDataFormat ( void ) {
  return dataFormat;
}

/**
 * @return The application API URL for proxyserver, e.g.: developer.peoplepowerco.com/cloud/xml 
 */
const char *proxycli_getApplicationApiUrl ( void ) {
    snprintf(applicationApiUrl, sizeof(applicationApiUrl), "%s/%s", applicationApiBaseUrl, proxycli_getDataFormat());
    return applicationApiUrl;
}

/**
 * @param url The new application API base URL
 * @return The application API base URL which proxyserver is interactive with, e.g: developer.peoplepowerco.com/cloud
 */
const char *proxycli_setApplicationApiBaseUrl ( const char *url ) {
    strncpy(applicationApiBaseUrl, url, sizeof(applicationApiBaseUrl));
    return applicationApiBaseUrl;
}

/**
 * @return The application API base URL which proxyserver is interactive with, e.g: developer.peoplepowerco.com/cloud
 */
const char *proxycli_getApplicationApiBaseUrl ( void ) {
    return applicationApiBaseUrl;
}


/**
 * @return The activation username
 */
const char *proxycli_getUsername() {
  return username;
}

/**
 * @return The activation password
 */
const char *proxycli_getPassword() {
  return password;
}

/**
 * @return true if we have enough information to activate
 */
const bool proxycli_readyToActivate() {
  if(username != NULL && password != NULL) {
    return true;
  }

  if(activationKey != NULL) {
    return true;
  }

  return false;
}

/***************** Private Functions ****************/
/**
 * Instruct the user how to use the application
 */
static void _proxycli_printUsage() {
  char *usage = ""
    "Usage: ./proxyserver (options)\n"
    "\t[-u username] : Activation username\n"
    "\t[-p password] : Activation password\n"
    "\t[-n port] : Define the port to open the proxy on\n"
    "\t[-c filename] : The name of the configuration file for the proxy\n"
    "\t[-l cloud_name] : The name of the Cloud the proxy want to interaction with\n"
    //"\t[-f format] : The data format that proxy used to interaction with FabrUX, xml or json\n"
    //"\t[-a key] : Activate this proxy using the given activation key and exit\n"
    "\t[-v] : Print version information\n"
    "\t[-?] : Print this menu\n"
    "\n"
    "\tUse the syslogs (usually /var/log/messages) to check the runtime status.\n";

  printf("%s", usage);
  SYSLOG_INFO("%s", usage);
}

/**
 * Print the version number
 */
static void _proxycli_printVersion() {
  printf("Built on %s at %s\n", __DATE__, __TIME__);
  printf("Git repository version %x\n", GIT_FIRMWARE_VERSION);
}
