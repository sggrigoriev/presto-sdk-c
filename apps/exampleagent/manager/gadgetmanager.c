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
 * This module manages the state of all gadgets in our network
 * @author David Moss
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "ioterror.h"
#include "iotdebug.h"

#include "gadgetmanager.h"
#include "gadgetagent.h"

/** Gadgets we are currently tracking */
static gadget_t devices[GADGET_MAX_DEVICES];

/***************** Public Functions ****************/
error_t gadgetmanager_add(gadget_t *gadget) {
  int i;

  for(i = 0; i < GADGET_MAX_DEVICES; i++) {
    if(devices[i].inUse && strcmp(gadget->ip, devices[i].ip) == 0) {
      SYSLOG_INFO("[gadget] Device at IP %s is already being tracked", gadget->ip);
      return SUCCESS;
    }
  }

  for(i = 0; i < GADGET_MAX_DEVICES; i++) {
    if(!devices[i].inUse) {
      SYSLOG_INFO("[gadget] Adding new device at ip %s!", gadget->ip);

      // Always add the device to the ESP
      iotxml_addDevice(gadget->uuid, GADGET_DEVICE_TYPE);

      memcpy(&devices[i], gadget, sizeof(gadget_t));
      devices[i].inUse = true;
      gadgetagent_refreshDevices();
      return SUCCESS;
    }
  }

  return FAIL;
}


/**
 * Obtain a gadget by IP address
 * @param ip IP address of a device
 * @return gadget_t if the device is being tracked
 */
gadget_t *gadgetmanager_getByIp(const char *ip) {
  int i;
  for(i = 0; i < GADGET_MAX_DEVICES; i++) {
    if(strncmp(ip, devices[i].ip, EUI64_STRING_SIZE) == 0) {
      return &devices[i];
    }
  }

  return NULL;
}


/**
 * Obtain a device by UUID
 * @param uuid UUID of a device
 * @return gadget_t if we are tracking this device
 */
gadget_t *gadgetmanager_getByUuid(const char *uuid) {
  int i;
  for(i = 0; i < GADGET_MAX_DEVICES; i++) {
    if(strncmp(uuid, devices[i].uuid, EUI64_STRING_SIZE) == 0) {
      return &devices[i];
    }
  }

  return NULL;
}


/**
 * @return the number of elements in our device array
 */
int gadgetmanager_size() {
  return GADGET_MAX_DEVICES;
}

/**
 * @param index Index into the array of devices
 * @return the device at the given index
 */
gadget_t *gadgetmanager_get(int index) {
  if(index < GADGET_MAX_DEVICES) {
    return &devices[index];
  } else {
    return NULL;
  }
}

/**
 * Kill off the devices we haven't heard from in a long time
 */
void gadgetmanager_garbageCollection() {
  int i;
  struct timeval curTime = { 0, 0 };

  gettimeofday(&curTime, NULL);

  for(i = 0; i < GADGET_MAX_DEVICES; i++) {
    if(devices[i].inUse) {
      if ((curTime.tv_sec - devices[i].lastTouchTime.tv_sec) >= GADGET_DEATH_PERIOD_SEC) {
        SYSLOG_INFO("[gadget] Killing device at IP %s", devices[i].ip);

        // Alert that the device is gone
        iotxml_alertDeviceIsGone(devices[i].uuid);

        bzero(&devices[i], sizeof(gadget_t));
      }
    }
  }
}
