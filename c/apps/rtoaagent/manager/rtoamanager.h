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



#ifndef RTOAMANAGER_H
#define RTOAMANAGER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "eui64.h"
#include "ioterror.h"

/**
 * Maximum number of thermostats can be set at compile time
 */
#ifndef RTOA_MAX_THERMOSTATS
#define RTOA_MAX_THERMOSTATS 8
#endif

/* Size of a full week's JSON program with 4 digits in each element */
#define RTOA_PROGRAM_SIZE 400

/** Amount of time after we haven't heard from an RTOA that we think it's dead */
#define RTOA_DEATH_PERIOD_SEC 300

/**
 * Information about each thermostat
 */
typedef struct rtoa_t {

  /** True if this slot is in use */
  bool inUse;

  /** True if this thermostat's measurements have been updated recently */
  bool measurementsUpdated;

  /** Time of the last successful measurement */
  struct timeval lastTouchTime;

  /** IP address of the thermostat */
  char ip[INET6_ADDRSTRLEN];

  /** Unique device ID */
  char uuid[EUI64_STRING_SIZE];

  /** Model description of the thermostat */
  char model[32];

  /** API version */
  int apiVersion;

  /** Firmware version */
  char firmwareVersion[8];

  /** WLAN Firmware Version */
  char wlanFirmwareVersion[12];

  /** Current temperature */
  double temp;

  /** Thermostat mode setting, though the thermostat may not actually be on */
  int tmode;

  /** Fan mode setting, though the fan may not actually be on */
  int fmode;

  /** Hold is turned on when the user has permanently turned off the schedule */
  int hold;

  /** Override is on when the user has manually adjusted the thermostat */
  int override;

  /** Heat target temperature */
  double heat;

  /** Cool target temperature */
  double cool;

  /** Thermostat state, != 0 means ON. Not available on all thermostats */
  int tstate;

  /** Fan state, != 0 means ON. Only available on the CT-30 */
  int fstate;

  /** Cool schedule */
  char programCool[RTOA_PROGRAM_SIZE];

  /** Heat schedule */
  char programHeat[RTOA_PROGRAM_SIZE];

} rtoa_t;


/**
 * Thermostat error codes
 */
typedef enum {
    RTOA_SUCCESS = 0,
    RTOA_FAIL = -1,
    RTOA_PARSE_ERROR = 10,
} rtoa_error_t;

/**
 * Tmode settings
 */
typedef enum {
    RTOA_THERMOSTAT_MODE_OFF = 0,
    RTOA_THERMOSTAT_MODE_HEAT = 1,
    RTOA_THERMOSTAT_MODE_COOL = 2,
    RTOA_THERMOSTAT_MODE_AUTO = 3,
} thermostat_mode_t;

/**
 * Fmode settings
 */
typedef enum {
  RTOA_FAN_MODE_AUTO = 0,
  RTOA_FAN_MODE_AUTO_CIRCULATE = 1,
  RTOA_FAN_MODE_ON = 2,
} fan_mode_t;

/**
 * Tstate settings
 */
typedef enum {
  RTOA_THERMOSTAT_STATE_OFF = 0,
  RTOA_THERMOSTAT_STATE_HEAT = 1,
  RTOA_THERMOSTAT_STATE_COOL = 2,
} thermostat_state_t;

/**
 * Fstate settings
 */
typedef enum {
  RTOA_FAN_STATE_OFF = 0,
  RTOA_FAN_STATE_ON = 1,
} fan_state_t;


/***************** Public Prototypes ****************/
error_t rtoamanager_add(rtoa_t *rtoa);

rtoa_t *rtoamanager_getByIp(const char *ip);

rtoa_t *rtoamanager_getByUuid(const char *uuid);

int rtoamanager_size();

rtoa_t *rtoamanager_get(int index);

void rtoamanager_garbageCollection();

#endif
