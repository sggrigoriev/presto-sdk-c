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
 * This module manages the state of all WEMO insights in our network
 * @author David Moss
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "ioterror.h"
#include "iotdebug.h"

#include "wemomanager.h"
#include "wemoagent.h"

/** Thermostats we are currently tracking */
static wemo_t insights[WEMO_MAX_INSIGHTS];

/***************** Public Functions ****************/
error_t wemomanager_add(wemo_t *wemo) {
  int i;

  for(i = 0; i < WEMO_MAX_INSIGHTS; i++) {
    if(insights[i].inUse && strcmp(wemo->ip, insights[i].ip) == 0) {
      SYSLOG_INFO("[wemo] Thermostat at IP %s is already being tracked", wemo->ip);
      return SUCCESS;
    }
  }

  for(i = 0; i < WEMO_MAX_INSIGHTS; i++) {
    if(!insights[i].inUse) {
      SYSLOG_INFO("[wemo] Adding new WeMo at ip %s!", wemo->ip);

      // Always add the device to the ESP
      iotxml_addDevice(wemo->uuid, WEMO_DEVICE_TYPE);

      memcpy(&insights[i], wemo, sizeof(wemo_t));
      insights[i].inUse = true;
      wemoagent_refreshDevices();
      return SUCCESS;
    }
  }

  return FAIL;
}


/**
 * Obtain a thermostat by IP address
 * @param ip IP address of a thermostat
 * @return wemo_t if the thermostat is being tracked
 */
wemo_t *wemomanager_getByIp(const char *ip) {
  int i;
  for(i = 0; i < WEMO_MAX_INSIGHTS; i++) {
    if(strncmp(ip, insights[i].ip, EUI64_STRING_SIZE) == 0) {
      return &insights[i];
    }
  }

  return NULL;
}


/**
 * Obtain a thermostat by UUID
 * @param uuid UUID of a thermostat
 * @return wemo_t if we are tracking this thermostat
 */
wemo_t *wemomanager_getByUuid(const char *uuid) {
  int i;
  for(i = 0; i < WEMO_MAX_INSIGHTS; i++) {
    if(strncmp(uuid, insights[i].uuid, EUI64_STRING_SIZE) == 0) {
      return &insights[i];
    }
  }

  return NULL;
}


/**
 * @return the number of elements in our device array
 */
int wemomanager_size() {
  return WEMO_MAX_INSIGHTS;
}

/**
 * @param index Index into the array of insights
 * @return the thermostat at the given index
 */
wemo_t *wemomanager_get(int index) {
  if(index < WEMO_MAX_INSIGHTS) {
    return &insights[index];
  } else {
    return NULL;
  }
}

/**
 * Kill off the insights we haven't heard from in a long time
 */
void wemomanager_garbageCollection() {
  int i;
  struct timeval curTime = { 0, 0 };

  gettimeofday(&curTime, NULL);

  for(i = 0; i < WEMO_MAX_INSIGHTS; i++) {
    if(insights[i].inUse) {
      if ((curTime.tv_sec - insights[i].lastTouchTime.tv_sec) >= WEMO_DEATH_PERIOD_SEC) {
        insights[i].inUse = false;

        // Alert that this device is gone
        iotxml_alertDeviceIsGone(insights[i].uuid);

        SYSLOG_INFO("[wemo] Killing thermostat at IP %s", insights[i].ip);
        memset(&insights[i], 0x0, sizeof(wemo_t));
      }
    }
  }
}
