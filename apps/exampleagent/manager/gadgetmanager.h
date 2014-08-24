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

#ifndef GADGETMANAGER_H
#define GADGETMANAGER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "eui64.h"
#include "ioterror.h"

/**
 * Maximum number of gadgets can be set at compile time
 */
#ifndef GADGET_MAX_DEVICES
#define GADGET_MAX_DEVICES 8
#endif

/** Amount of time after we haven't heard from an gadget that we think it's dead */
#define GADGET_DEATH_PERIOD_SEC 600

/**
 * Information about each gadget
 */
typedef struct gadget_t {

  /** True if this slot is in use */
  bool inUse;

  /** True if this gadget's measurements have been updated recently */
  bool measurementsUpdated;

  /** Time of the last successful measurement */
  struct timeval lastTouchTime;

  /** IP address of the gadget */
  char ip[INET6_ADDRSTRLEN];

  /** Unique device ID */
  char uuid[EUI64_STRING_SIZE];

  /** Model description of the gadget */
  char model[32];

  /** Firmware version */
  char firmwareVersion[8];

  /** True if the outlet is on */
  bool isOn;

  /** Current in amps */
  double current_amps;

  /** Power in watts */
  double power_watts;

  /** Voltage */
  double voltage;

  /** Power factor */
  int powerFactor;

  /** Accumulated energy in watt-hours */
  double energy_wh;

} gadget_t;


/***************** Public Prototypes ****************/
error_t gadgetmanager_add(gadget_t *gadget);

gadget_t *gadgetmanager_getByIp(const char *ip);

gadget_t *gadgetmanager_getByUuid(const char *uuid);

int gadgetmanager_size();

gadget_t *gadgetmanager_get(int index);

void gadgetmanager_garbageCollection();

#endif
