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
#include "cJSON.h"
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
static void parse_object(cJSON *item);

static void _login_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts);

static void _login_xml_charactersHandler(void *ctx, const xmlChar *ch, int len);

static int resultCode = -1;
static char key[PATH_MAX];
static char keyExpire[PATH_MAX];

/***************** Public Functions ***************/
/**
 * Login
 * @param username
 * @param password
 * @return Application API key, or NULL if we couldn't log in
 */
error_t login_doLogin(const char *username, const char *password) {
  char url[PATH_MAX];
  char rxBuffer[PROXY_MAX_MSG_LEN];
  char headerPassword[PROXY_HEADER_PASSWORD_LEN];
  http_param_t params;
  login_info_t loginInfo;

  bzero(&params, sizeof(params));

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

  snprintf(url, sizeof(url), "%s/login?username=%s", proxycli_getApplicationApiUrl(), username);
  snprintf(headerPassword, sizeof(headerPassword), "PASSWORD: %s", password);

  printf("Logging in...\n");
  SYSLOG_INFO("Logging in...");
  printf("Contacting URL %s\n", url);
  SYSLOG_INFO("Contacting URL %s\n", url);

  params.verbose = TRUE;
  params.timeouts.connectTimeout = HTTPCOMM_DEFAULT_CONNECT_TIMEOUT_SEC;
  params.timeouts.transferTimeout = HTTPCOMM_DEFAULT_TRANSFER_TIMEOUT_SEC;
  params.password = headerPassword;

  libhttpcomm_sendMsg(NULL, CURLOPT_HTTPGET, url, NULL, NULL, NULL, 0, rxBuffer, sizeof(rxBuffer), params, NULL);

  SYSLOG_INFO("Server returned: \n%s\n", rxBuffer);

  loginInfo.resultCode = -1;

  if ( strcmp(proxycli_getDataFormat(), "xml") == 0 )
  {
      if ( 0 == xmlSAXUserParseMemory(&saxHandler, &loginInfo, rxBuffer, strlen(rxBuffer)) )
      {

	  if(loginInfo.resultCode == 0) {
	      printf("Login successful!\n");
	      SYSLOG_INFO("Login successful");
	      return SUCCESS;

	  } else {
	      printf("Error logging in\n");
	      return FAIL;
	  }
      }
  }
  else if ( strcmp(proxycli_getDataFormat(), "json") == 0 )
  {
      cJSON *root = cJSON_Parse(rxBuffer);
      parse_object(root);
      cJSON_Delete(root);

      if ( resultCode == 0 && strlen(key) > 0 && strlen(keyExpire) > 0 ) {
	  SYSLOG_INFO("resultCode: %d key: %s keyExpire: %s", resultCode, key, keyExpire);
	  printf("Login successful!\n");
	  SYSLOG_INFO("Login successful");
	  return SUCCESS;

      } else {
	  printf("Error logging in\n");
	  return FAIL;
      }
  }
  else
  {
      SYSLOG_ERR("Invalid data format: %s", proxycli_getDataFormat());
  }


  printf("Error logging in\n");
  return FAIL;
}


/**
 * @return the API key if it exists, NULL if it doesn't
 */
char *login_getApiKey() {
  return apiKey;
}

/***************** Private Functions ****************/
/**
 * Parse a JSON item
 */
static void parse_object(cJSON *item)
{
    if ( cJSON_GetArraySize(item) > 0 )
    {
	int i = 0;
	resultCode = -1;
	key[0] = 0;
	keyExpire[0] = 0;


	for (i=0;i<cJSON_GetArraySize(item);i++)
	{
	    cJSON *subitem=cJSON_GetArrayItem(item,i);
	    // handle subitem.	
	    if ( subitem->string != NULL )
	    {

		if ( strcmp(subitem->string, "resultCode") == 0) {
		    resultCode = subitem->valueint;
		}
		else if ( strcmp(subitem->string, "key") == 0) {
		    strncpy(key, subitem->valuestring, sizeof(key));
		}
		else if ( strcmp(subitem->string, "keyExpire") == 0) {
		    strncpy(keyExpire, subitem->valuestring, sizeof(keyExpire));
		}
	    }
	}

    }
}

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
      libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_API_KEY, apiKey);
    } else {
      SYSLOG_DEBUG("Login does not support XML tag %s", loginInfo->xmlTag);
    }

  } else {
    SYSLOG_ERR("Received an XML value that is too long to parse");
  }
}




