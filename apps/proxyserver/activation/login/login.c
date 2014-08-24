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
#include "login.h"
#include "proxycli.h"
#include "ioterror.h"
#include "iotdebug.h"
#include "proxycli.h"
#include "proxy.h"


/** API key */
static char apiKey[API_KEY_LENGTH];

/***************** Private Prototypes ****************/
static void _login_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts);

static void _login_xml_charactersHandler(void *ctx, const xmlChar *ch, int len);


/***************** Public Functions ***************/
/**
 * Login
 * @param username
 * @param password
 * @return Application API key, or NULL if we couldn't log in
 */
error_t login_doLogin(const char *username, const char *password) {
  char baseUrl[PATH_MAX];
  char url[PATH_MAX];
  char rxBuffer[PROXY_MAX_MSG_LEN];
  http_param_t params;
  login_info_t loginInfo;

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
      _login_xml_startElementHandler, // startElement
      NULL, // endElement
      NULL, // reference,
      _login_xml_charactersHandler, //characters
      NULL, // ignorableWhitespace
      NULL, // processingInstructionHandler,
      NULL, // comment
      NULL, // warning
      NULL, // error
      NULL, // fatal
  };

  // Read the activation URL from the configuration file
  if(libconfigio_read(proxycli_getConfigFilename(), CONFIGIO_ACTIVATION_URL_TOKEN_NAME, baseUrl, sizeof(baseUrl)) == -1) {
    printf("Couldn't read %s in file %s, writing default value\n", CONFIGIO_ACTIVATION_URL_TOKEN_NAME, proxycli_getConfigFilename());
    libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_ACTIVATION_URL_TOKEN_NAME, DEFAULT_ACTIVATION_URL);
    strncpy(baseUrl, DEFAULT_ACTIVATION_URL, sizeof(baseUrl));
  }

  snprintf(url, sizeof(url), "%s/login/%s/%s/14", baseUrl, username, password);

  SYSLOG_INFO("Logging in...");
  SYSLOG_INFO("Contacting URL %s\n", url);

  params.verbose = TRUE;
  params.timeouts.connectTimeout = HTTPCOMM_DEFAULT_CONNECT_TIMEOUT_SEC;
  params.timeouts.transferTimeout = HTTPCOMM_DEFAULT_TRANSFER_TIMEOUT_SEC;

  libhttpcomm_sendMsg(NULL, CURLOPT_HTTPGET, url, NULL, NULL, NULL, 0, rxBuffer, sizeof(rxBuffer), params, NULL);

  SYSLOG_INFO("Server returned: \n%s\n", rxBuffer);

  loginInfo.resultCode = -1;
  xmlSAXUserParseMemory(&saxHandler, &loginInfo, rxBuffer, strlen(rxBuffer));

  if(loginInfo.resultCode == 0) {
    printf("Login successful!\n");
    SYSLOG_INFO("Login successful");
    return SUCCESS;

  } else {
    printf("Error logging in\n");
    return FAIL;
  }
}


/**
 * @return the API key if it exists, NULL if it doesn't
 */
char *login_getApiKey() {
  return apiKey;
}

/***************** Private Functions ****************/
/**
 * Parse a new XML tag
 */
static void _login_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts) {
    // Know which tag we're grabbing character data out of
    snprintf(((login_info_t *) ctx)->xmlTag, LOGIN_XML_TAG_SIZE, "%s", (char *) name);
}

/**
 * Parse the characters found as the value of an XML tag
 */
static void _login_xml_charactersHandler(void *ctx, const xmlChar *ch, int len) {
  int i;
  char output[API_KEY_LENGTH];
  login_info_t *loginInfo = (login_info_t *) ctx;

  if (len > 0 && len < API_KEY_LENGTH) {
    for (i = 0; i < len; i++) {
      //if not equal a LF, CR store the character
      if ((ch[i] != 10) && (ch[i] != 13)) {
        output[i] = ch[i];
      }
    }
    output[i] = '\0';

    if (strcmp(loginInfo->xmlTag, "keyExpire") == 0) {
      // Tag is ok, but do nothing

    } else if (strcmp(loginInfo->xmlTag, "resultCode") == 0) {
      loginInfo->resultCode = atoi(output);

    } else if (strcmp(loginInfo->xmlTag, "key") == 0) {
      snprintf(apiKey, sizeof(apiKey), "%s", output);

    } else {
      SYSLOG_DEBUG("Login does not support XML tag %s", loginInfo->xmlTag);
    }

  } else {
    SYSLOG_ERR("Received an XML value that is too long to parse");
  }
}




