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
 * This module is responsible for activating the proxy with the server,
 * which will link the physical instance of this proxy to a group or username.
 * There are multiple ways to do this, and this activation functionality
 * here is only one of those multiple ways.
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
#include "getactivationinfo.h"
#include "proxycli.h"
#include "ioterror.h"
#include "iotdebug.h"
#include "proxycli.h"
#include "proxy.h"
#include "../login/login.h"



/** Device activation key */
static char activationKey[ACTIVATION_KEY_LENGTH];

/** True if the activation key is valid */
static bool activationKeyValid = false;


/***************** Private Prototypes ****************/
static void _getactivationinfo_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts);

static void _getactivationinfo_xml_charactersHandler(void *ctx, const xmlChar *ch, int len);

static void _registrationinfo_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts);

static void _registrationinfo_xml_charactersHandler(void *ctx, const xmlChar *ch, int len);


/***************** Public Functions ***************/
/**
 * Get the activation key from the server
 * @param key Application API key from logging in
 * @param locationId The location ID for this user
 * @return the activation key for this device
 */
char *getactivationinfo_getDeviceActivationKey(const char *key, int locationId) {
  char url[PATH_MAX];
  char baseUrl[PATH_MAX];
  char deviceType[8];
  char rxBuffer[PROXY_MAX_MSG_LEN];
  char headerApiKey[PROXY_HEADER_KEY_LEN];
  http_param_t params;
  getactivationinfo_info_t getActivationInfo;

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
      _getactivationinfo_xml_startElementHandler, // startElement
      NULL, // endElement
      NULL, // reference,
      _getactivationinfo_xml_charactersHandler, //characters
      NULL, // ignorableWhitespace
      NULL, // processingInstructionHandler,
      NULL, // comment
      NULL, // warning
      NULL, // error
      NULL, // fatal
  };

  if(activationKeyValid) {
    return activationKey;
  }


  bzero(deviceType, sizeof(deviceType));
  bzero(&params, sizeof(params));
  snprintf(headerApiKey, sizeof(headerApiKey), "FABRUX_API_KEY: %s", login_getApiKey());

  // Read the device type from the configuration file
  if(libconfigio_read(proxycli_getConfigFilename(), CONFIGIO_PROXY_DEVICE_TYPE_TOKEN_NAME, deviceType, sizeof(deviceType)) == -1) {
    printf("Couldn't read %s in file %s, writing default value\n", CONFIGIO_PROXY_DEVICE_TYPE_TOKEN_NAME, proxycli_getConfigFilename());
    libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_PROXY_DEVICE_TYPE_TOKEN_NAME, DEFAULT_PROXY_DEVICETYPE);
    strncpy(deviceType, DEFAULT_PROXY_DEVICETYPE, sizeof(deviceType));
  }

  // Read the activation URL from the configuration file
  if(libconfigio_read(proxycli_getConfigFilename(), CONFIGIO_ACTIVATION_URL_TOKEN_NAME, baseUrl, sizeof(baseUrl)) == -1) {
    printf("Couldn't read %s in file %s, writing default value\n", CONFIGIO_ACTIVATION_URL_TOKEN_NAME, proxycli_getConfigFilename());
    libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_ACTIVATION_URL_TOKEN_NAME, DEFAULT_ACTIVATION_URL);
    strncpy(baseUrl, DEFAULT_ACTIVATION_URL, sizeof(baseUrl));
  }

  snprintf(url, sizeof(url), "%s/locations/%d/deviceActivation/%s", baseUrl, locationId, deviceType);

  //SYSLOG_INFO("Getting device activation key...");
  SYSLOG_INFO("Contacting URL %s\n", url);

  params.verbose = TRUE;
  params.timeouts.connectTimeout = HTTPCOMM_DEFAULT_CONNECT_TIMEOUT_SEC;
  params.timeouts.transferTimeout = HTTPCOMM_DEFAULT_TRANSFER_TIMEOUT_SEC;
  params.key = headerApiKey;

  libhttpcomm_sendMsg(NULL, CURLOPT_HTTPGET, url, NULL, NULL, NULL, 0, rxBuffer, sizeof(rxBuffer), params, NULL);

  SYSLOG_INFO("Server returned: \n%s\n", rxBuffer);

  getActivationInfo.resultCode = -1;
  xmlSAXUserParseMemory(&saxHandler, &getActivationInfo, rxBuffer, strlen(rxBuffer));

  if(getActivationInfo.resultCode == 0) {
    printf("Downloaded the secret activation key!\n");
    activationKeyValid = true;
    return activationKey;

  } else {
    printf("Error getting activation key. Check the syslogs.\n");
    return NULL;
  }
}

/**
 * Register Device
 * @return 
 */
error_t registerDevice(void) {
  char url[PATH_MAX];
  char baseUrl[PATH_MAX];
  char deviceType[8];
  char rxBuffer[PROXY_MAX_MSG_LEN];
  char headerApiKey[PROXY_HEADER_KEY_LEN];
  char eui64[EUI64_STRING_SIZE+8];
  http_param_t params;
  registrationinfo_info_t registrationInfo;

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
      _registrationinfo_xml_startElementHandler, // startElement
      NULL, // endElement
      NULL, // reference,
      _registrationinfo_xml_charactersHandler, //characters
      NULL, // ignorableWhitespace
      NULL, // processingInstructionHandler,
      NULL, // comment
      NULL, // warning
      NULL, // error
      NULL, // fatal
  };

  bzero(deviceType, sizeof(deviceType));
  bzero(&params, sizeof(params));
  snprintf(headerApiKey, sizeof(headerApiKey), "FABRUX_API_KEY: %s", login_getApiKey());

  // Read the device type from the configuration file
  if(libconfigio_read(proxycli_getConfigFilename(), CONFIGIO_PROXY_DEVICE_TYPE_TOKEN_NAME, deviceType, sizeof(deviceType)) == -1) {
    printf("Couldn't read %s in file %s, writing default value\n", CONFIGIO_PROXY_DEVICE_TYPE_TOKEN_NAME, proxycli_getConfigFilename());
    libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_PROXY_DEVICE_TYPE_TOKEN_NAME, DEFAULT_PROXY_DEVICETYPE);
    strncpy(deviceType, DEFAULT_PROXY_DEVICETYPE, sizeof(deviceType));
  }

  // Read the activation URL from the configuration file
  if(libconfigio_read(proxycli_getConfigFilename(), CONFIGIO_ACTIVATION_URL_TOKEN_NAME, baseUrl, sizeof(baseUrl)) == -1) {
    printf("Couldn't read %s in file %s, writing default value\n", CONFIGIO_ACTIVATION_URL_TOKEN_NAME, proxycli_getConfigFilename());
    libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_ACTIVATION_URL_TOKEN_NAME, DEFAULT_ACTIVATION_URL);
    strncpy(baseUrl, DEFAULT_ACTIVATION_URL, sizeof(baseUrl));
  }

  eui64_toString(eui64, sizeof(eui64));
  // https://developer.presencepro.com/cloud/json/devices/001C42DE23CF-4-33F?productId=4
  snprintf(url, sizeof(url), "%s/devices/%s?productId=%s", baseUrl, eui64, deviceType);

  SYSLOG_INFO("Register device...");
  SYSLOG_INFO("Contacting URL %s\n", url);

  params.verbose = TRUE;
  params.timeouts.connectTimeout = HTTPCOMM_DEFAULT_CONNECT_TIMEOUT_SEC;
  params.timeouts.transferTimeout = HTTPCOMM_DEFAULT_TRANSFER_TIMEOUT_SEC;
  params.key = headerApiKey;

  libhttpcomm_postMsg(NULL, CURLOPT_HTTPPOST, url, NULL, NULL, "", 0, rxBuffer, sizeof(rxBuffer), params, NULL);

  SYSLOG_INFO("Server returned: \n%s\n", rxBuffer);

  registrationInfo.resultCode = -1;

  printf("rxBuffer:(%s)\n", rxBuffer);
  if ( 0 == xmlSAXUserParseMemory(&saxHandler, &registrationInfo, rxBuffer, strlen(rxBuffer)) )
  {

      if(registrationInfo.resultCode == 0) {
	  printf("Register device successful!\n");
	  SYSLOG_INFO("Register device successful");
	  return SUCCESS;

      } else {
	  printf("Error register device\n");
	  return FAIL;
      }
  }

  printf("Error register device\n");
  return FAIL;
}


/***************** Private Functions ****************/
/**
 * Parse a new XML tag
 */
static void _getactivationinfo_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts) {
    // Know which tag we're grabbing character data out of
    snprintf(((getactivationinfo_info_t *) ctx)->xmlTag, GETACTIVATIONINFO_XML_TAG_SIZE, "%s", (char *) name);
}

/**
 * Parse the characters found as the value of an XML tag
 */
static void _getactivationinfo_xml_charactersHandler(void *ctx, const xmlChar *ch, int len) {
  int i;
  char output[ACTIVATION_KEY_LENGTH];
  getactivationinfo_info_t *getActivationInfo = (getactivationinfo_info_t *) ctx;

  if (len > 0 && len < ACTIVATION_KEY_LENGTH) {
    for (i = 0; i < len; i++) {
      //if not equal a LF, CR store the character
      if ((ch[i] != 10) && (ch[i] != 13)) {
        output[i] = ch[i];
      }
    }
    output[i] = '\0';

    if (strcmp(getActivationInfo->xmlTag, "resultCode") == 0) {
      getActivationInfo->resultCode = atoi(output);

    } else if (strcmp(getActivationInfo->xmlTag, "deviceActivationKey") == 0) {
      snprintf(activationKey, sizeof(activationKey), "%s", output);

    } else {
      SYSLOG_DEBUG("Activation does not support XML tag %s", getActivationInfo->xmlTag);
    }

  } else {
    SYSLOG_ERR("Received an XML value that is too long to parse");
  }
}


/**
 * Parse a new XML tag
 */
static void _registrationinfo_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts) {
    // Know which tag we're grabbing character data out of
    snprintf(((registrationinfo_info_t *) ctx)->xmlTag, GETACTIVATIONINFO_XML_TAG_SIZE, "%s", (char *) name);
}

/**
 * Parse the characters found as the value of an XML tag
 */
static void _registrationinfo_xml_charactersHandler(void *ctx, const xmlChar *ch, int len) {
  int i;
  char output[ACTIVATION_KEY_LENGTH];
  registrationinfo_info_t *registrationInfo = (registrationinfo_info_t *) ctx;

  if (len > 0 && len < ACTIVATION_KEY_LENGTH) {
    for (i = 0; i < len; i++) {
      //if not equal a LF, CR store the character
      if ((ch[i] != 10) && (ch[i] != 13)) {
        output[i] = ch[i];
      }
    }
    output[i] = '\0';

    if (strcmp(registrationInfo->xmlTag, "resultCode") == 0) {
      registrationInfo->resultCode = atoi(output);

    } else if (strcmp(registrationInfo->xmlTag, "host") == 0) {
      libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_CLOUD_HOST, output);

    } else if (strcmp(registrationInfo->xmlTag, "port") == 0) {
      libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_CLOUD_PORT, output);

    } else if (strcmp(registrationInfo->xmlTag, "uri") == 0) {
      libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_CLOUD_URI, output);

    } else if (strcmp(registrationInfo->xmlTag, "useSSL") == 0) {
      libconfigio_write(proxycli_getConfigFilename(), CONFIGIO_CLOUD_USE_SSL, output);

    } else {
      SYSLOG_DEBUG("Registration does not support XML tag %s", registrationInfo->xmlTag);
    }
  } else {
    SYSLOG_ERR("Received an XML value that is too long to parse");
  }
}




