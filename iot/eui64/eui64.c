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


#include <assert.h>
#include <curl/curl.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <rpc/types.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "ioterror.h"
#include "iotdebug.h"
#include "eui64.h"
#include "proxyserver.h"

/**
 * Obtain the 48-bit MAC dest and convert to an EUI-64 value from the
 * hardware NIC
 *
 * @param dest Buffer of at least 8 bytes
 * @param destLen Length of the buffer
 * @return SUCCESS if we are able to capture the EUI64
 */
error_t eui64_toBytes(uint8_t *dest, int destLen) {
  struct ifreq *ifr;
  struct ifconf ifc;
  char buf[1024];
  int sock, i;
  int ok = 0;


  assert(dest);

  if(destLen < EUI64_BYTES_SIZE) {
    return FAIL;
  }

  memset(dest, 0x0, destLen);

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1) {
    return -1;
  }

  ifc.ifc_len = sizeof(buf);
  ifc.ifc_buf = buf;
  ioctl(sock, SIOCGIFCONF, &ifc);

  ifr = ifc.ifc_req;
  for (i = 0; i < ifc.ifc_len / sizeof(struct ifreq); ifr++) {
      if (strcmp(ifr->ifr_name, "eth0") == 0 || strcmp(ifr->ifr_name, "eth1")
	      == 0 || strcmp(ifr->ifr_name, "wlan0") == 0 || strcmp(ifr->ifr_name,
		  "br0") == 0) {
	  if (ioctl(sock, SIOCGIFFLAGS, ifr) == 0) {
	      if (!(ifr->ifr_flags & IFF_LOOPBACK)) {
		  if (ioctl(sock, SIOCGIFHWADDR, ifr) == 0) {
		      ok = 1;
		      break;
		  }
	      }
	  }
      }
  }

  close(sock);
  if (ok) {
    /* Convert 48 bit MAC dest to EUI-64 */
    memcpy(dest, ifr->ifr_hwaddr.sa_data, 6);
    /* Insert the converting bits in the middle */
    /* dest[3] = 0xFF;
    dest[4] = 0xFE;
    memcpy(&dest[5], &(ifr->ifr_hwaddr.sa_data[3]), 3);
    */
  } else {
    SYSLOG_ERR("Couldn't read MAC dest to seed EUI64");
    return FAIL;
  }

  return SUCCESS;
}

/**
 * Copy the EUI64 into a string
 * @return SUCCESS if we are able to capture the EUI64
 */
extern char *argEui64Bytes;
error_t eui64_toString(char *dest, int destLen) {
  uint8_t byteAddress[EUI64_BYTES_SIZE], i;
  uint16_t checksum= 0;
  char deviceType[DEVICE_TYPE_SIZE];
  char *tmp = NULL;
  extern char* argDeviceType;

  assert(dest);

  if(destLen < EUI64_STRING_SIZE) {
    return FAIL;
  }

  /* new format: ${MAC_ADDRESS}-${PRODUCT_ID}-${CHECKSUM} */
  if (eui64_toBytes(byteAddress, sizeof(byteAddress)) == SUCCESS) {

    memset(deviceType, 0x0, DEVICE_TYPE_SIZE);
    readDeviceType(deviceType);

    if ( argDeviceType != NULL )
    {
	strncpy(deviceType, argDeviceType, sizeof(deviceType));
    }

    memset(dest, 0x0, destLen);
    if ( argEui64Bytes != NULL )
    {
	snprintf(dest, destLen, "%s-%s-",
		argEui64Bytes, deviceType);
    }
    else
    {
	snprintf(dest, destLen, "%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X-%s-",
		byteAddress[0], byteAddress[1], byteAddress[2], byteAddress[3],
		byteAddress[4], byteAddress[5], deviceType);
    }

    for(i = 0; i < strlen(dest) ; i++ ) {
        checksum+= dest[i];
    }

    tmp = strdup(dest);

    if ( tmp != NULL )
    {
	snprintf(dest, destLen, "%s%X", tmp, checksum);
	free(tmp);
    }


    return SUCCESS;
  }

  return FAIL;
}

/**
 * Read deviceType via proxyserver.h
 * 
 * @return deviceType if can get via proxy.conf or 
 */
error_t readDeviceType(char *deviceType)
{
/**
 * #define DEFAULT_PROXY_DEVICETYPE "4"
 * #define DEFAULT_PROXY_CONFIG_FILENAME "/opt/etc/proxy.conf"
 * #define CONFIGIO_PROXY_DEVICE_TYPE_TOKEN_NAME "PROXY_DEVICE_TYPE"
 */
#if defined(DEFAULT_PROXY_CONFIG_FILENAME) && defined(CONFIGIO_PROXY_DEVICE_TYPE_TOKEN_NAME)
  FILE *fp= fopen(DEFAULT_PROXY_CONFIG_FILENAME,"r");
  char line[1024];

  if ( deviceType[0] != 0 )
  {
      return SUCCESS;
  }
  
  if ( fp!= NULL ) {
    while(fgets(line, sizeof(line), fp)) {
      if ( !strncmp(line,CONFIGIO_PROXY_DEVICE_TYPE_TOKEN_NAME,strlen(CONFIGIO_PROXY_DEVICE_TYPE_TOKEN_NAME)) ) {
	line[strlen(line)-1]= 0;
	snprintf(deviceType, DEVICE_TYPE_SIZE, "%s", line+strlen(CONFIGIO_PROXY_DEVICE_TYPE_TOKEN_NAME)+1);
        break;
      }
    }
    fclose(fp);
  }
#endif
  
  if ( deviceType[0]==0x0 ) {
#ifdef DEFAULT_PROXY_DEVICETYPE
    snprintf(deviceType, DEVICE_TYPE_SIZE, "%s", DEFAULT_PROXY_DEVICETYPE);
#else
    deviceType[0]= '0';
    return FAIL;
#endif
  }

  return SUCCESS;
}
