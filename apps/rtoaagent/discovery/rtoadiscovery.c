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
 * Discovery RTOA thermostats on the local sub-net passively in the background
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

#include "cJSON.h"
#include "libhttpcomm.h"
#include "ioterror.h"
#include "iotdebug.h"
#include "rtoadiscovery.h"
#include "rtoamanager.h"
#include "rtoaagent.h"


/***************** Private Prototypes ***************/
static void _rtoadiscovery_ssdpHandler(char *url);

static void _rtoadiscovery_poll(const char *ip);

static error_t _rtoadiscovery_captureRtoaDetails(rtoa_t *rtoa);

/***************** Public Functions ***************/
/**
 * Discover new RTOA devices
 *
 * @param discovery_Handler: handler that is called when a new device is discovered
 */
error_t rtoadiscovery_runOnce() {
  int sock;
  int ret;
  int one = 1;
  int ttl = 3;
  unsigned int len;
  struct sockaddr_in cliaddr;
  struct sockaddr_in destaddr;
  struct timeval tv;
  struct timeval curTime;
  char buffer[RTOA_MAX_MSG_SIZE] = "TYPE: WM-DISCOVER\r\nVERSION:1.0\r\n\r\nservices: com.marvell.wm.system*\r\n\r\n";
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
     * RTOA discovery is found to intermittently prevent the resolution of
     * host names on the router.  When this happens, we are unable to bind
     * to this port. Since we can detect this error, we reboot the system
     * when it occurs.
     *
     * Since the problem is intermittent, the work around here is to
     * rarely perform discovery and reboot if there's a problem.
     */
#warning "RTOA will reboot the system if discover prevents host name resolution"
    if(system("reboot &") < 0) {
      SYSLOG_ERR("System returned %s for reboot", strerror(errno));
    }
#endif
    return FAIL;
  }

  /* construct an IGMP join request structure */
  mc_req.imr_multiaddr.s_addr = inet_addr(RTOA_SSDP_ADDR);
  mc_req.imr_interface.s_addr = htonl(INADDR_ANY);

  /* send an ADD MEMBERSHIP message via setsockopt */
  if ((setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*) &mc_req,
      sizeof(mc_req))) < 0) {
    SYSLOG_ERR("%s, setsockopt() failed", strerror(errno));
    return FAIL;
  }

  /* Set destination for multicast address */
  destaddr.sin_family = AF_INET;
  destaddr.sin_addr.s_addr = inet_addr(RTOA_SSDP_ADDR);
  destaddr.sin_port = htons(RTOA_SSDP_PORT);

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
    ret = recvfrom(sock, buffer, RTOA_MAX_MSG_SIZE, 0, (struct sockaddr *) &destaddr, &len);

    if (ret > 0) {
      count++;
      /* Valid response */
      /* Parse the response */
      SYSLOG_INFO("Got a response!");

      token = strtok(buffer, "\r\n");

      while (token != NULL) {
        if (!strncasecmp(token, RTOA_LOCATION_HDR, strlen(RTOA_LOCATION_HDR))) {
          SYSLOG_DEBUG("Found a wireless microcontroller, base URI: %s", token + strlen(RTOA_LOCATION_HDR));
          _rtoadiscovery_ssdpHandler(token + strlen(RTOA_LOCATION_HDR));
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
static void _rtoadiscovery_ssdpHandler(char *url) {
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

    _rtoadiscovery_poll(ip);
  }
}


/**
 * Poll to verify whether a device is there
 * @param ip IP addr to poll
 */
void _rtoadiscovery_poll(const char *ip) {
  rtoa_t rtoa;
  rtoa_t *rtoaPtr;

  assert(ip);

  if ((rtoaPtr = rtoamanager_getByIp(ip)) == NULL) {
    rtoaPtr = &rtoa;
    memset(rtoaPtr, 0x0, sizeof(rtoa_t));
    strcpy(rtoaPtr->ip, ip);
    SYSLOG_INFO("[rtoa] Creating new rtoa thermostat with IP %s", rtoaPtr->ip);

  } else {
    SYSLOG_INFO("[rtoa] Refreshing rtoa thermostat IP %s", rtoaPtr->ip);
  }

  if(_rtoadiscovery_captureRtoaDetails(rtoaPtr) == SUCCESS) {
    rtoamanager_add(rtoaPtr);
  }
}

/**
 * Capture the details of a thermostat at a particular address
 * @param rtoa Pointer to an rtoa_t struct that has the IP address of the RTOA
 * @return SUCCESS if it's a thermostat, FAIL if it's not an RTOA thermostat
 */
static error_t _rtoadiscovery_captureRtoaDetails(rtoa_t *rtoa) {
  char url[PATH_MAX];
  char rxBuffer[RTOA_MAX_MSG_SIZE];
  http_param_t params;
  cJSON *jsonMsg = NULL;
  cJSON *jsonObject = NULL;

  assert(rtoa);

  params.verbose = false;
  params.timeouts.connectTimeout = 3;
  params.timeouts.transferTimeout = 15;

  snprintf(url, sizeof(url), "http://%s/tstat/model", rtoa->ip);
  if (libhttpcomm_getMsg(NULL, url, NULL, NULL, rxBuffer, sizeof(rxBuffer), params, NULL) == 0) {
    if ((jsonMsg = cJSON_Parse(rxBuffer)) != NULL) {
      if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_MODEL)) != NULL) {
        strcpy(rtoa->model, jsonObject->valuestring);

      } else {
        // This doesn't look like a thermostat to me
        return FAIL;
      }
    }
  }

  snprintf(url, sizeof(url), "http://%s/sys", rtoa->ip);
  if (libhttpcomm_getMsg(NULL, url, NULL, NULL, rxBuffer, sizeof(rxBuffer), params, NULL) == 0) {
    if ((jsonMsg = cJSON_Parse(rxBuffer)) != NULL) {

      if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_UUID)) != NULL) {
        strcpy(rtoa->uuid, jsonObject->valuestring);
      }

      if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_API_VERSION)) != NULL) {
        rtoa->apiVersion = jsonObject->valueint;
      }

      if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_FW_VERSION)) != NULL) {
        strcpy(rtoa->firmwareVersion, jsonObject->valuestring);
      }

      if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_WLAN_FW_VERSION)) != NULL) {
        strcpy(rtoa->wlanFirmwareVersion, jsonObject->valuestring);
      }
    }
  }

  SYSLOG_INFO("[rtoa] ip=%s; model=%s; uuid=%s; apiVersion=%d; firmwareVersion=%s; wlanFirmwareVersion=%s", rtoa->ip, rtoa->model, rtoa->uuid, rtoa->apiVersion, rtoa->firmwareVersion, rtoa->wlanFirmwareVersion);

  return SUCCESS;
}

