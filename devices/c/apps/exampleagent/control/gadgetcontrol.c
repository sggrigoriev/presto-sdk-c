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
 * This module implements commands from the server
 *
 * The function here, gadgetconrol_execute(command_t *cmd), is called because
 * our main gadgetagent.c file added the function as a listener to the XML
 * parser.  When the XML parser is done parsing a new command, this function
 * is called with a pointer to the command struct.
 *
 * @author Yvan Castilloux
 * @author David Moss
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#include <cJSON.h>

#include "libhttpcomm.h"

#include "ioterror.h"
#include "iotdebug.h"
#include "proxy.h"
#include "gadgetagent.h"
#include "gadgetmanager.h"
#include "gadgetmeasure.h"
#include "iotapi.h"


/***************** Public Functions ****************/
/**
 * All commands and acks get broadcast to all agents. Therefore, we must
 * filter out all the commands that don't belong to us, and only execute
 * the commands that we have a device for and we know how to execute.
 *
 * EXAMPLE ONLY!
 */
void gadgetcontrol_execute(command_t *cmd) {
  gadget_t *focusedGadget;

  if(cmd->userIsWatching) {
    // The user is watching! Send out measurements asap!
    gadgetagent_refreshDevices();
  }

  // 1. Filter the command by device ID.
  if((focusedGadget = gadgetmanager_getByUuid(cmd->deviceId)) != NULL) {

    // 2. I have this device. Filter the command by name
    if(strcmp(cmd->commandName, "outletStatus") == 0) {
      char url[PATH_MAX];
      char rxBuffer[GADGET_MAX_MSG_SIZE];
      http_param_t params;

      params.verbose = false;
      params.timeouts.connectTimeout = 3;
      params.timeouts.transferTimeout = 15;

      // 3. This is a command I know how to execute!
      snprintf(url, sizeof(url), "%s/set.xml/%d", focusedGadget->ip, atoi(cmd->argument));
      if(0 != libhttpcomm_getMsg(NULL, url, NULL, NULL, rxBuffer, sizeof(rxBuffer), params, NULL)) {
        iotxml_sendResult(cmd->commandId, IOT_RESULT_DEVICECONNECTIONERROR);
        return;
      }

      if(strstr(rxBuffer, "success") == NULL) {
        iotxml_sendResult(cmd->commandId, IOT_RESULT_DEVICEEXECUTIONERROR);
        return;
      }

      iotxml_sendResult(cmd->commandId, IOT_RESULT_EXECUTED);

      // We just executed the command, refresh the measurements quickly
      gadgetagent_refreshDevices();
    }
  }
}
