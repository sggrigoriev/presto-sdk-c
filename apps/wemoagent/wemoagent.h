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


#ifndef WEMOAGENT_H
#define WEMOAGENT_H

#include <pthread.h>

#include "wemocontrol.h"
#include "wemomanager.h"
#include "wemodiscovery.h"
#include "wemoheartbeat.h"
#include "wemomeasure.h"

/** WEMO Device Type as defined on the server */
#define WEMO_DEVICE_TYPE 10121

/** Number of seconds between discovery attempts */
#define WEMO_DISCOVERY_PERIOD_SEC 60

/** Number of seconds between heartbeats */
#define WEMO_HEARTBEAT_PERIOD_SEC 300

/** Number of seconds between measurements */
#define WEMO_MEASUREMENT_PERIOD_SEC 60

/** Number of seconds between capturing the program schedules on thermostats */
#define WEMO_CAPTURE_PROGRAM_PERIOD_SEC 3600

/** Number of seconds between time synchronizations */
#define WEMO_TIME_SYNC_PERIOD_SEC 3600

/** Maximum size of a message buffer to receive messages from the thermostat */
#define WEMO_MAX_MSG_SIZE 1024

/***************** Public Prototypes ****************/
void wemoagent_setHeartbeatPeriod(int seconds);

void wemoagent_setMeasurementPeriod(int seconds);

void wemoagent_refreshDevices();

void wemoagent_discover();

pthread_mutex_t *wemoagent_getMutex();

#endif

