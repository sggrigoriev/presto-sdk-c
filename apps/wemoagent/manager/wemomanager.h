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



#ifndef WEMOMANAGER_H
#define WEMOMANAGER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "eui64.h"
#include "ioterror.h"

/**
 * Maximum number of insights can be set at compile time
 */
#ifndef WEMO_MAX_INSIGHTS
#define WEMO_MAX_INSIGHTS 8
#endif

/* Size of a full week's JSON program with 4 digits in each element */
#define WEMO_PROGRAM_SIZE 400

/** Amount of time after we haven't heard from an WEMO that we think it's dead */
#define WEMO_DEATH_PERIOD_SEC 300

/**
 * Information about each thermostat
 */
typedef struct wemo_t {

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

  /** Current watts */
  double watts;

} wemo_t;


/**
 * Wemo error codes
 */
typedef enum {
    WEMO_SUCCESS = 0,
    WEMO_FAIL = -1,
    WEMO_PARSE_ERROR = 10,
} wemo_error_t;

/**
 * Fstate settings
 */
typedef enum {
  WEMO_FAN_STATE_OFF = 0,
  WEMO_FAN_STATE_ON = 1,
} fan_state_t;


/***************** Public Prototypes ****************/
error_t wemomanager_add(wemo_t *wemo);

wemo_t *wemomanager_getByIp(const char *ip);

wemo_t *wemomanager_getByUuid(const char *uuid);

int wemomanager_size();

wemo_t *wemomanager_get(int index);

void wemomanager_garbageCollection();

#endif
