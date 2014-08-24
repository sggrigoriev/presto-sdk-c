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


#ifndef PROXYAGENT_H
#define PROXYAGENT_H

#include "ioterror.h"

#ifndef PROXY_AGENT_HEARTBEAT_INTERVAL
#define PROXY_AGENT_HEARTBEAT_INTERVAL 60
#endif

/** Parameter name for the firmware version */
#define PARAM_NAME_FIRMWARE_VERSION "firmware"

/** Parameter name for the amount of time this proxy has been active */
#define PARAM_NAME_ALIVE_TIME "aliveTime"

/** Parameter name for the number of times the proxy has restarted */
#define PARAM_NAME_REBOOTS "reboots"

/** Parameter name for the passive upload interval of the proxy */
#define PARAM_NAME_UPLOAD_INTERVAL "uploadInterval"


/***************** Public Prototypes ****************/
error_t proxyagent_start();

void proxyagent_stop();

#endif
