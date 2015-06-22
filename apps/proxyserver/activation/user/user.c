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
 * This module is responsible for logging into the server with a username
 * and password, and returns an Application API key.
 *
 * @author David Moss
 */


#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>
#include <rpc/types.h>
#include <stdio.h>

#include <libxml/parser.h>

#include "libhttpcomm.h"
#include "libconfigio.h"
#include "eui64.h"
#include "proxyserver.h"
#include "proxycli.h"
#include "ioterror.h"
#include "iotdebug.h"
#include "proxycli.h"
#include "proxy.h"
#include "user.h"
#include "../login/login.h"


/** API key */
static int locationId = -1;

/***************** Private Prototypes ****************/
static void _user_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts);


/***************** Public Functions ***************/
/**
 * Get user info and extract the location ID
 *
 * @param key API key
 * @return Location ID, or -1 if we couldn't obtain it
 */
int user_getLocationId(const char *key) {
  char baseUrl[PATH_MAX];
  char url[PATH_MAX];
  char rxBuffer[PROXY_MAX_MSG_LEN];
  char headerApiKey[PROXY_HEADER_KEY_LEN];
  http_param_t params;

  xmlSAXHandler saxHandler = {
      NULL, // internalSubsetHandler,
      NULL, // isStandaloneHandler,
      NULL, // hasInternalSubsetHandler,
      NULL, // hasExternalSubsetHandler,
      NULL, // resolveEntityHandler,
      NULL, // getEntityHandler,
      NULL, // entityDeclHandler,
      NULL, // notationDeclHandler,
      NULL, // attributeDeclHandler,
      NULL, // elementDeclHandler,
      NULL, // unparsedEntityDeclHandler,
      NULL, // setDocumentLocatorHandler,
      NULL, // startDocument
      NULL, // endDocument
      _user_xml_startElementHandler, // startElement
      NULL, // endElement
      NULL, // reference,
      NULL, //characters
      NULL, // ignorableWhitespace
      NULL, // processingInstructionHandler,
      NULL, // comment
      NULL, // warning
      NULL, // error
      NULL, // fatal
  };

  if(locationId != -1) {
    return locationId;
  }

  bzero(&params, sizeof(params));
  snprintf(headerApiKey, sizeof(headerApiKey), "FABRUX_API_KEY: %s", login_getApiKey());

  // Read the activation URL from the configuration file
  if(libconfigio_read(proxycli_getConfigFilename(), CONFIGIO_ACTIVATION_URL_TOKEN_NAME, baseUrl, sizeof(baseUrl)) == -1) {
    printf("Couldn't read %s in file %s, writing default value\n", CONFIGIO_ACTIVATION_URL_TOKEN_NAME, proxycli_getConfigFilename());
    libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_ACTIVATION_URL_TOKEN_NAME, DEFAULT_ACTIVATION_URL);
    strncpy(baseUrl, DEFAULT_ACTIVATION_URL, sizeof(baseUrl));
  }

  snprintf(url, sizeof(url), "%s/user", baseUrl);

  SYSLOG_INFO("Obtaining location ID...");
  SYSLOG_INFO("Contacting URL %s\n", url);

  params.verbose = TRUE;
  params.timeouts.connectTimeout = HTTPCOMM_DEFAULT_CONNECT_TIMEOUT_SEC;
  params.timeouts.transferTimeout = HTTPCOMM_DEFAULT_TRANSFER_TIMEOUT_SEC;
  params.key = headerApiKey;

  libhttpcomm_sendMsg(NULL, CURLOPT_HTTPGET, url, NULL, NULL, NULL, 0, rxBuffer, sizeof(rxBuffer), params, NULL);

  SYSLOG_INFO("Server returned: \n%s\n", rxBuffer);

  xmlSAXUserParseMemory(&saxHandler, NULL, rxBuffer, strlen(rxBuffer));

  return locationId;
}


/***************** Private Functions ****************/
/**
 * Parse a new XML tag
 */
static void _user_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts) {
  int i;
  char *attr;
  char *value;

  if(strcmp((char *) name, "location") == 0) {
    if (atts != NULL) {
      for (i = 0; (atts[i] != NULL); i++) {
        attr = (char *) atts[i++];
        value = (char *) atts[i];

        if(strcmp(attr, "id") == 0) {
          locationId = atoi(value);
          printf("Found the location ID!\n");
          SYSLOG_INFO("Found location ID: %d", locationId);
        }
      }
    }
  }
}


