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

#ifndef GADGETAGENT_H
#define GADGETAGENT_H

#include "gadgetcontrol.h"
#include "gadgetmanager.h"
#include "gadgetdiscovery.h"
#include "gadgetheartbeat.h"
#include "gadgetmeasure.h"

/** Device Type as defined on the server */
#define GADGET_DEVICE_TYPE 3

/** Number of seconds betweeen discovery attempts */
#define GADGET_DISCOVERY_PERIOD_SEC 60

/** Number of seconds between heartbeats */
#define GADGET_HEARTBEAT_PERIOD_SEC 300

/** Number of seconds between measurements */
#define GADGET_MEASUREMENT_PERIOD_SEC 60

/** Maximum size of a message buffer to receive messages from the gadget */
#define GADGET_MAX_MSG_SIZE 1024

/***************** Public Prototypes ****************/
void gadgetagent_setHeartbeatPeriod(int seconds);

void gadgetagent_setMeasurementPeriod(int seconds);

void gadgetagent_refreshDevices();


#endif

