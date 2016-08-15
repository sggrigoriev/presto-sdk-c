/*
 * Copyright (c) 2011 People Power Company
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
 * Discovery WEMO insights on the local sub-net passively in the background
 * @author Yvan Castilloux
 * @author David Moss
 */

#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pthread.h>
#include <rpc/types.h>
#include <libxml/parser.h>

#include "cJSON.h"
#include "libhttpcomm.h"
#include "ioterror.h"
#include "iotdebug.h"
#include "wemodiscovery.h"
#include "wemomanager.h"
#include "wemoagent.h"


/***************** Private Prototypes ***************/
static void _wemodiscovery_ssdpHandler(char *url);
static void _user_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts);

static void _wemodiscovery_poll(const char *ip);

static error_t _wemodiscovery_captureWemoDetails(wemo_t *wemo);

/***************** Public Functions ***************/
/**
 * Discover new WEMO devices
 *
 * @param discovery_Handler: handler that is called when a new device is discovered
 */
error_t wemodiscovery_runOnce() {
  int sock;
  int ret;
  int one = 1;
  int ttl = 3;
  unsigned int len;
  struct sockaddr_in cliaddr;
  struct sockaddr_in destaddr;
  struct timeval tv;
  struct timeval curTime;
  char buffer[WEMO_MAX_MSG_SIZE] = "M-SEARCH * HTTP/1.1\r\nHOST:239.255.255.250:1900\r\nST:upnp:rootdevice\r\nMX:2\r\nMAN:\"ssdp:discover\"\r\n\r\n";
  char *token;
  int count = 0;
  struct ip_mreq mc_req;

  /* Create socket */
  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock < 0) {
    SYSLOG_ERR("Cannot open socket");
    return FAIL;
  }
  /* Allow socket resue */
  ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof(one));

  if (ret < 0) {
    SYSLOG_ERR("Cannot prepare socket for reusing");
    return FAIL;
  }

  if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1) {
    SYSLOG_ERR("fcntl(sock), %s", strerror(errno));
    return FAIL;
  }

  ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*) &ttl,
      sizeof(ttl));

  if (ret < 0) {
    SYSLOG_ERR("Cannot set ttl to the socket");
    return FAIL;
  }

  /* construct a socket bind address structure */
  cliaddr.sin_family = AF_INET;
  cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliaddr.sin_port = htons(0);

  ret = bind(sock, (struct sockaddr *) &cliaddr, sizeof(cliaddr));

  if (ret < 0) {
    SYSLOG_ERR("Cannot bind port");
#ifdef MIPS
    /*
     * WEMO discovery is found to intermittently prevent the resolution of
     * host names on the router.  When this happens, we are unable to bind
     * to this port. Since we can detect this error, we reboot the system
     * when it occurs.
     *
     * Since the problem is intermittent, the work around here is to
     * rarely perform discovery and reboot if there's a problem.
     */
#warning "WEMO will reboot the system if discover prevents host name resolution"
    if(system("reboot &") < 0) {
      SYSLOG_ERR("System returned %s for reboot", strerror(errno));
    }
#endif
    return FAIL;
  }

  /* construct an IGMP join request structure */
  mc_req.imr_multiaddr.s_addr = inet_addr(WEMO_SSDP_ADDR);
  mc_req.imr_interface.s_addr = htonl(INADDR_ANY);

  /* send an ADD MEMBERSHIP message via setsockopt */
  if ((setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*) &mc_req,
      sizeof(mc_req))) < 0) {
    SYSLOG_ERR("%s, setsockopt() failed", strerror(errno));
    return FAIL;
  }

  /* Set destination for multicast address */
  destaddr.sin_family = AF_INET;
  destaddr.sin_addr.s_addr = inet_addr(WEMO_SSDP_ADDR);
  destaddr.sin_port = htons(WEMO_SSDP_PORT);

  /* Send the multicast packet */
  len = strlen(buffer);
  ret = sendto(sock, buffer, len, 0, (struct sockaddr *) &destaddr,
      sizeof(destaddr));

  if (ret < 0) {
    SYSLOG_ERR("%s: Cannot send data", strerror(errno));
    return FAIL;
  }

  /* quiet the noise */
  SYSLOG_DEBUG("Sent the SSDP multicast request and now waiting for a response");
  gettimeofday(&curTime, NULL);
  tv.tv_sec = curTime.tv_sec;

  while ((curTime.tv_sec - tv.tv_sec) < 3) {
    gettimeofday(&curTime, NULL);

    /* Wait for response */
    len = sizeof(destaddr);
    ret = recvfrom(sock, buffer, WEMO_MAX_MSG_SIZE, 0, (struct sockaddr *) &destaddr, &len);

    if (ret > 0) {
      count++;
      /* Valid response */
      /* Parse the response */
      SYSLOG_INFO("Got a response!");

      token = strtok(buffer, "\r\n");

      while (token != NULL) {
        if (!strncasecmp(token, WEMO_LOCATION_HDR, strlen(WEMO_LOCATION_HDR))) {
          SYSLOG_DEBUG("Found a wireless microcontroller, base URI: %s", token + strlen(WEMO_LOCATION_HDR));
          _wemodiscovery_ssdpHandler(token + strlen(WEMO_LOCATION_HDR));
          break;
        }

        token = strtok(NULL, "\r\n");
      }

    } else {
      usleep(500000);
    }
  }

  /* send a DROP MEMBERSHIP message via setsockopt */
  if ((setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void*) &mc_req, sizeof(mc_req))) < 0) {
    SYSLOG_ERR("%s, setsockopt() failed", strerror(errno));
    return FAIL;
  }

  SYSLOG_DEBUG("Found %d device(s)", count);
  return SUCCESS;
}



/***************** Private Functions ****************/
/**
 * Function called by the SSDP discovery mechanism
 * @param ip: message received by the handler
 */
static void _wemodiscovery_ssdpHandler(char *url) {
  char ip[INET6_ADDRSTRLEN];
  char *ptr;
  char *ptr2;

  //TODO extract IP address from url
  ptr = strstr(url, "://");
  if (ptr != NULL) {
    ptr += 3; // advance pointer after "://"
    ptr2 = strchr(ptr, '/');
    if (ptr2 == NULL) {
      strncpy(ip, ptr, sizeof(ip));
    } else {
      strncpy(ip, ptr, ptr2 - ptr);
      ip[ptr2 - ptr] = 0;
    }

    _wemodiscovery_poll(ip);
  }
}


/**
 * Poll to verify whether a device is there
 * @param ip IP addr to poll
 */
void _wemodiscovery_poll(const char *ip) {
  wemo_t wemo;
  wemo_t *wemoPtr;

  assert(ip);

  if ((wemoPtr = wemomanager_getByIp(ip)) == NULL) {
    wemoPtr = &wemo;
    memset(wemoPtr, 0x0, sizeof(wemo_t));
    strcpy(wemoPtr->ip, ip);
    SYSLOG_INFO("[wemo] Creating new wemo insight with IP %s", wemoPtr->ip);

  } else {
    SYSLOG_INFO("[wemo] Refreshing wemo insight IP %s", wemoPtr->ip);
  }

  if(_wemodiscovery_captureWemoDetails(wemoPtr) == SUCCESS) {
    wemomanager_add(wemoPtr);
  }
}

/**
 * Capture the details of a insight at a particular address
 * @param wemo Pointer to an wemo_t struct that has the IP address of the WEMO
 * @return SUCCESS if it's a insight, FAIL if it's not an WEMO insight
 */
static error_t _wemodiscovery_captureWemoDetails(wemo_t *wemo) {
  char url[PATH_MAX];
  char rxBuffer[WEMO_MAX_MSG_SIZE];
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

  assert(wemo);

  params.verbose = false;
  params.timeouts.connectTimeout = 3;
  params.timeouts.transferTimeout = 15;

  snprintf(url, sizeof(url), "http://%s/setup.xml", wemo->ip);
  if (libhttpcomm_getMsg(NULL, url, NULL, NULL, rxBuffer, sizeof(rxBuffer), params, NULL) == 0) {
      xmlSAXUserParseMemory(&saxHandler, NULL, rxBuffer, strlen(rxBuffer));
#if 0
      if ((jsonMsg = cJSON_Parse(rxBuffer)) != NULL) {
	  if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_MODEL)) != NULL) {
	      strcpy(wemo->model, jsonObject->valuestring);

	  } else {
	      // This doesn't look like a insight to me
	      return FAIL;
	  }
      }
#endif
  }

  snprintf(url, sizeof(url), "http://%s/sys", wemo->ip);
  if (libhttpcomm_getMsg(NULL, url, NULL, NULL, rxBuffer, sizeof(rxBuffer), params, NULL) == 0) {
#if 0
    if ((jsonMsg = cJSON_Parse(rxBuffer)) != NULL) {

      if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_UUID)) != NULL) {
        strcpy(wemo->uuid, jsonObject->valuestring);
      }

      if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_API_VERSION)) != NULL) {
        wemo->apiVersion = jsonObject->valueint;
      }

      if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_FW_VERSION)) != NULL) {
        strcpy(wemo->firmwareVersion, jsonObject->valuestring);
      }

      if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_WLAN_FW_VERSION)) != NULL) {
        strcpy(wemo->wlanFirmwareVersion, jsonObject->valuestring);
      }
    }
#endif
  }

  SYSLOG_INFO("[wemo] ip=%s; model=%s; uuid=%s; apiVersion=%d; firmwareVersion=%s; wlanFirmwareVersion=%s", wemo->ip, wemo->model, wemo->uuid, wemo->apiVersion, wemo->firmwareVersion, wemo->wlanFirmwareVersion);

  return SUCCESS;
}

/**
 * Parse a new XML tag
 */
static void _user_xml_startElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts) {
    int i = 0;
    char *attr = NULL;
    char *value = NULL;

    if ( strcmp((char *) name, "cloud") == 0 )
    {
	if (atts != NULL) {
	    for (i = 0; (atts[i] != NULL); i++) {
		attr = (char *) atts[i++];
		value = (char *) atts[i];

		if(strcmp(attr, "name") == 0) {
		}
	    }
	}
    }
}


