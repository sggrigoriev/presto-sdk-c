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
 * Send heartbeats for all the thermostats that are alive
 * @author David Moss
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ioterror.h"
#include "iotdebug.h"
#include "proxy.h"
#include "wemoheartbeat.h"
#include "wemoagent.h"
#include "iotapi.h"



/***************** Public Functions ****************/
/**
 * Send a heartbeat for each device
 */
void wemoheartbeat_send() {
  char myMsg[PROXY_MAX_MSG_LEN];
  int i;
  int offset = 0;
  int totalHeartbeats = 0;
  wemo_t *focusedRtoa;

  iotxml_newMsg(myMsg, sizeof(myMsg));

  for(i = 0; i < wemomanager_size(); i++) {
    if((focusedRtoa = wemomanager_get(i)) != NULL) {
      if(focusedRtoa->inUse) {
        totalHeartbeats++;

        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_PROFILE,
            "internalUrl",
            NULL,
            0,
            focusedRtoa->ip);

        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_PROFILE,
            "model",
            NULL,
            0,
            focusedRtoa->model);

        offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_PROFILE,
            "apiVersion",
            NULL,
            0,
            focusedRtoa->apiVersion);

        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_PROFILE,
            "firmware",
            NULL,
            0,
            focusedRtoa->firmwareVersion);

        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_PROFILE,
            "wlanFirmwareVersion",
            NULL,
            0,
            focusedRtoa->wlanFirmwareVersion);
      }
    }
  }

  if(totalHeartbeats == 0) {
    iotxml_abortMsg();

  } else {
    iotxml_send(myMsg, sizeof(myMsg));
  }
}

