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
 * This module manages the state of all RTOA thermostats in our network
 * @author David Moss
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "ioterror.h"
#include "iotdebug.h"

#include "rtoamanager.h"
#include "rtoaagent.h"

/** Thermostats we are currently tracking */
static rtoa_t thermostats[RTOA_MAX_THERMOSTATS];

/***************** Public Functions ****************/
error_t rtoamanager_add(rtoa_t *rtoa) {
  int i;

  for(i = 0; i < RTOA_MAX_THERMOSTATS; i++) {
    if(thermostats[i].inUse && strcmp(rtoa->ip, thermostats[i].ip) == 0) {
      SYSLOG_INFO("[rtoa] Thermostat at IP %s is already being tracked", rtoa->ip);
      return SUCCESS;
    }
  }

  for(i = 0; i < RTOA_MAX_THERMOSTATS; i++) {
    if(!thermostats[i].inUse) {
      SYSLOG_INFO("[rtoa] Adding new thermostat at ip %s!", rtoa->ip);

      // Always add the device to the ESP
      iotxml_addDevice(rtoa->uuid, RTOA_DEVICE_TYPE);

      memcpy(&thermostats[i], rtoa, sizeof(rtoa_t));
      thermostats[i].inUse = true;
      rtoaagent_refreshDevices();
      return SUCCESS;
    }
  }

  return FAIL;
}


/**
 * Obtain a thermostat by IP address
 * @param ip IP address of a thermostat
 * @return rtoa_t if the thermostat is being tracked
 */
rtoa_t *rtoamanager_getByIp(const char *ip) {
  int i;
  for(i = 0; i < RTOA_MAX_THERMOSTATS; i++) {
    if(strncmp(ip, thermostats[i].ip, EUI64_STRING_SIZE) == 0) {
      return &thermostats[i];
    }
  }

  return NULL;
}


/**
 * Obtain a thermostat by UUID
 * @param uuid UUID of a thermostat
 * @return rtoa_t if we are tracking this thermostat
 */
rtoa_t *rtoamanager_getByUuid(const char *uuid) {
  int i;
  for(i = 0; i < RTOA_MAX_THERMOSTATS; i++) {
    if(strncmp(uuid, thermostats[i].uuid, EUI64_STRING_SIZE) == 0) {
      return &thermostats[i];
    }
  }

  return NULL;
}


/**
 * @return the number of elements in our device array
 */
int rtoamanager_size() {
  return RTOA_MAX_THERMOSTATS;
}

/**
 * @param index Index into the array of thermostats
 * @return the thermostat at the given index
 */
rtoa_t *rtoamanager_get(int index) {
  if(index < RTOA_MAX_THERMOSTATS) {
    return &thermostats[index];
  } else {
    return NULL;
  }
}

/**
 * Kill off the thermostats we haven't heard from in a long time
 */
void rtoamanager_garbageCollection() {
  int i;
  struct timeval curTime = { 0, 0 };

  gettimeofday(&curTime, NULL);

  for(i = 0; i < RTOA_MAX_THERMOSTATS; i++) {
    if(thermostats[i].inUse) {
      if ((curTime.tv_sec - thermostats[i].lastTouchTime.tv_sec) >= RTOA_DEATH_PERIOD_SEC) {
        thermostats[i].inUse = false;

        // Alert that this device is gone
        iotxml_alertDeviceIsGone(thermostats[i].uuid);

        SYSLOG_INFO("[rtoa] Killing thermostat at IP %s", thermostats[i].ip);
        memset(&thermostats[i], 0x0, sizeof(rtoa_t));
      }
    }
  }
}
