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
#include "settings.h"


/***************** Private Prototypes ****************/
static void _user_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts);


static char currentCloudName[PATH_MAX];
static char lookupCloudName[PATH_MAX];
/***************** Public Functions ***************/
/**
 * Get FabrUX connection settings for specified device ID
 *
 * @param deviceId device ID
 * @return Location ID, or -1 if we couldn't obtain it
 */
error_t getConnectionSettings ( const char *deviceId, const char *cloudName ) {
  char baseUrl[PATH_MAX];
  char url[PATH_MAX];
  char rxBuffer[PROXY_MAX_MSG_LEN];
  http_param_t params;

  strncpy(lookupCloudName, cloudName, sizeof(lookupCloudName));
  strncpy(currentCloudName, "", sizeof(currentCloudName));

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

  bzero(&params, sizeof(params));

  // Read the activation URL from the configuration file
  strncpy(baseUrl, DEFAULT_ACTIVATION_URL, sizeof(baseUrl));

  snprintf(url, sizeof(url), "%s/settings?deviceId=%s", baseUrl, deviceId);

  SYSLOG_INFO("Obtaining connection settings for device ID %s...", deviceId);
  SYSLOG_INFO("Contacting URL %s\n", url);

  params.verbose = TRUE;
  params.timeouts.connectTimeout = HTTPCOMM_DEFAULT_CONNECT_TIMEOUT_SEC;
  params.timeouts.transferTimeout = HTTPCOMM_DEFAULT_TRANSFER_TIMEOUT_SEC;

  libhttpcomm_sendMsg(NULL, CURLOPT_HTTPGET, url, NULL, NULL, NULL, 0, rxBuffer, sizeof(rxBuffer), params, NULL);

  SYSLOG_INFO("Server returned: \n%s\n", rxBuffer);

  xmlSAXUserParseMemory(&saxHandler, NULL, rxBuffer, strlen(rxBuffer));

  return SUCCESS;
}


/***************** Private Functions ****************/
/**
 * Parse a new XML tag
 */
static void _user_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts) {
  int i;
  char *attr;
  char *value;

  if(strcmp((char *) name, "cloud") == 0 ) {
    if (atts != NULL) {
      for (i = 0; (atts[i] != NULL); i++) {
        attr = (char *) atts[i++];
        value = (char *) atts[i];

        if(strcmp(attr, "name") == 0) {
	    strncpy(currentCloudName, value, sizeof(currentCloudName));
        }
      }
    }
  }

  if ( strcmp((char *) name, "server") == 0 && strcmp(currentCloudName, lookupCloudName) == 0 ) {
    if (atts != NULL) {
	char altPort[PATH_MAX], altSsl[PATH_MAX], host[PATH_MAX], port[PATH_MAX], ssl[PATH_MAX], type[PATH_MAX], version[PATH_MAX];
	altPort[0] = 0; altSsl[0] = 0; host[0] = 0; port[0] = 0; ssl[0] = 0; type[0] = 0; version[0] = 0;
	for (i = 0; (atts[i] != NULL); i++) {
	    attr = (char *) atts[i++];
	    value = (char *) atts[i];

	    if(strcmp(attr, "altPort") == 0) {
		strncpy(altPort, value, sizeof(altPort));
	    }
	    else if(strcmp(attr, "altSsl") == 0) {
		strncpy(altSsl, value, sizeof(altSsl));
	    }
	    else if(strcmp(attr, "host") == 0) {
		strncpy(host, value, sizeof(host));
	    }
	    else if(strcmp(attr, "port") == 0) {
		strncpy(port, value, sizeof(port));
	    }
	    else if(strcmp(attr, "ssl") == 0) {
		strncpy(ssl, value, sizeof(ssl));
	    }
	    else if(strcmp(attr, "type") == 0) {
		strncpy(type, value, sizeof(type));
	    }
	    else if(strcmp(attr, "version") == 0) {
		strncpy(version, value, sizeof(version));
	    }

	    if ( strlen(altPort) > 0 && strlen(altSsl) > 0 && strlen(host) > 0 && strlen(type) > 0 )
	    {
		if ( strncmp(type, "appapi", sizeof(type)) == 0 )
		{
		    // DEFAULT_ACTIVATION_URL "developer.peoplepowerco.com/cloud/xml"
		    char baseUrl[PATH_MAX];
		    snprintf(baseUrl, sizeof(baseUrl), "%s:%s/%s", host, altPort, DEFAULT_XML_API_BASE_URL);
		    libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_ACTIVATION_URL_TOKEN_NAME, baseUrl);

		    printf("Application API: %s\n", baseUrl);
		    SYSLOG_INFO("Application API: %s", baseUrl);

		    return;
		}
	    }
	}
    }
  }
}


