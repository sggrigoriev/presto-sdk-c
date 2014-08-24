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
 * Send heartbeats for all the gadgets that are alive.
 *
 * Heartbeats are sent at least once per hour and contain all the static
 * non-changing metadata information about each device.
 *
 * @author David Moss
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ioterror.h"
#include "iotdebug.h"
#include "proxy.h"
#include "gadgetheartbeat.h"
#include "gadgetagent.h"
#include "iotapi.h"



/***************** Public Functions ****************/
/**
 * Send a heartbeat for each device.
 *
 * Here I create a new message, add the XML describing all devices in the for()
 * loop, and then send the complete message with all device information at
 * the end.
 *
 * If I don't have any devices under my control, I end up aborting the
 * message. Alternatively, you might want to implement a
 * int gadgetmanager_totalDevices() function that would let us abort the
 * message before we start it, if we don't have any devices under our control.
 *
 * EXAMPLE ONLY!
 */
void gadgetheartbeat_send() {
  char myMsg[PROXY_MAX_MSG_LEN];
  int i;
  int offset = 0;
  int totalHeartbeats = 0;
  gadget_t *focusedGadget;

  iotxml_newMsg(myMsg, sizeof(myMsg));

  for(i = 0; i < gadgetmanager_size(); i++) {
    if((focusedGadget = gadgetmanager_get(i)) != NULL) {
      if(focusedGadget->inUse) {
        totalHeartbeats++;

        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedGadget->uuid,
            GADGET_DEVICE_TYPE,
            IOT_PARAM_PROFILE,
            "internalUrl",
            NULL,
            0,
            focusedGadget->ip);

        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedGadget->uuid,
            GADGET_DEVICE_TYPE,
            IOT_PARAM_PROFILE,
            "model",
            NULL,
            0,
            focusedGadget->model);

        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedGadget->uuid,
            GADGET_DEVICE_TYPE,
            IOT_PARAM_PROFILE,
            "firmware",
            NULL,
            0,
            focusedGadget->firmwareVersion);
      }
    }
  }

  if(totalHeartbeats == 0) {
    iotxml_abortMsg();

  } else {
    iotxml_send(myMsg, sizeof(myMsg));
  }
}

