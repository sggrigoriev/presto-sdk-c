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
 * This module takes measurements from all WEMO thermostats in our network
 * @author Yvan Castilloux
 * @author David Moss
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "libhttpcomm.h"
#include "cJSON.h"

#include "ioterror.h"
#include "iotdebug.h"
#include "proxy.h"
#include "wemomanager.h"
#include "wemomeasure.h"
#include "wemoagent.h"
#include "iotapi.h"


/***************** Private Prototypes ****************/

/***************** Public Functions ****************/
/**
 * Capture measurements for all known thermostats
 * We will drop all measurements that have a -1 in them, since that's an error
 */
void wemomeasure_capture() {
  int i;
  wemo_t wemoBuffer;
  wemo_t *focusedWemo;
  char url[PATH_MAX];
  char rxBuffer[WEMO_MAX_MSG_SIZE];
  http_param_t params;
  cJSON *jsonMsg = NULL;
  cJSON *jsonObject = NULL;
  struct timeval curTime = { 0, 0 };

  params.verbose = false;
  params.timeouts.connectTimeout = 3;
  params.timeouts.transferTimeout = 15;

  /*
   * We keep a buffer of measurements until we know all measurements are good.
   * Then at the end, we copy all the measurements from the buffer to the actual
   * focusedWemo pointer.
   *
   * The WEMO has been observed sending back -1's from time to time. If we
   * encounter one of these, we drop the entire measurement and move on to the
   * next one.
   */
  for(i = 0; i < wemomanager_size(); i++) {
    if((focusedWemo = wemomanager_get(i)) != NULL) {
      if(focusedWemo->inUse) {
        // If a value doesn't exist on the thermostat, it appears as -1 at the server
        memset(&wemoBuffer, -1, sizeof(wemoBuffer));

        snprintf(url, sizeof(url), "http://%s/tstat", focusedWemo->ip);

        if (libhttpcomm_getMsg(NULL, url, NULL, NULL, rxBuffer, sizeof(rxBuffer), params, NULL) == 0) {
          SYSLOG_DEBUG("http://%s/tstat returned: %s", focusedWemo->ip, rxBuffer);

#if 0
          if ((jsonMsg = cJSON_Parse(rxBuffer)) != NULL) {

            // Temperature
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_TEMP)) != NULL) {
              if(jsonObject->valueint != -1) {
                wemoBuffer.temp = jsonObject->valuedouble;
              } else {
                continue;
              }
            }

            // Tmode
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_TMODE)) != NULL) {
              if(jsonObject->valueint != -1) {
                wemoBuffer.tmode = jsonObject->valueint;
              } else {
                continue;
              }
            }

            // Fmode
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_FMODE)) != NULL) {
              if(jsonObject->valueint != -1) {
                wemoBuffer.fmode = jsonObject->valueint;
              } else {
                continue;
              }
            }

            // Hold
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_HOLD)) != NULL) {
              if(jsonObject->valueint != -1) {
                wemoBuffer.hold = jsonObject->valueint;
              } else {
                continue;
              }
            }

            // Override
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_OVERRIDE)) != NULL) {
              if(jsonObject->valueint != -1) {
                wemoBuffer.override = jsonObject->valueint;
              } else {
                continue;
              }
            }

            // Heat
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_HEAT)) != NULL) {
              if(jsonObject->valueint != -1) {
                wemoBuffer.heat = jsonObject->valuedouble;
              } else {
                continue;
              }
            } else {
              // We aren't in heater mode
              wemoBuffer.heat = 0;
            }

            // Cool
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_COOL)) != NULL) {
              if(jsonObject->valueint != -1) {
                wemoBuffer.cool = jsonObject->valuedouble;
              } else {
                continue;
              }
            } else {
              // We aren't in cooler mode
              wemoBuffer.cool = 0;
            }

            // Tstate
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_TSTATE)) != NULL) {
              if(jsonObject->valueint != -1) {
                wemoBuffer.tstate = jsonObject->valueint;
              } else {
                continue;
              }
            }

            // Fstate
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, WEMO_JSON_ATTR_FSTATE)) != NULL) {
              if(jsonObject->valueint != -1) {
                wemoBuffer.fstate = jsonObject->valueint;
              } else {
                continue;
              }
            }

            focusedWemo->temp = wemoBuffer.temp;
            focusedWemo->tmode = wemoBuffer.tmode;
            focusedWemo->fmode = wemoBuffer.fmode;
            focusedWemo->hold = wemoBuffer.hold;
            focusedWemo->override = wemoBuffer.override;
            focusedWemo->heat = wemoBuffer.heat;
            focusedWemo->cool = wemoBuffer.cool;
            focusedWemo->tstate = wemoBuffer.tstate;
            focusedWemo->fstate = wemoBuffer.fstate;
            focusedWemo->measurementsUpdated = true;

            gettimeofday(&curTime, NULL);
            focusedWemo->lastTouchTime.tv_sec = curTime.tv_sec;
          }
#endif 
        }
      }
    }
  }
}

/**
 * Capture the schedule of the thermostat
 *
 * We can only get the program when the user hasn't manually
 * set the temperature on his own, because getting the
 * program will remove the user's settings and start using
 * the programmed schedule's temperture settings instead.
 * (1.04.64 thermostat firmware version)
 *
 * This will make sure the thermotat is not in override mode before
 * capturing its schedule.  We recommend the developer only calls this function
 * after capturing the rest of the measurements off of the thermostat.
 * Also, don't call this one too often because it interfers with a user
 * interacting with the thermostat manually
 */
void wemomeasure_captureSchedules() {
  int i;
  wemo_t *focusedWemo;
  char url[PATH_MAX];
  char rxBuffer[WEMO_MAX_MSG_SIZE];
  http_param_t params;

  params.verbose = false;
  params.timeouts.connectTimeout = 3;
  params.timeouts.transferTimeout = 15;

  /*
   * We keep a buffer of measurements until we know all measurements are good.
   * Then at the end, we copy all the measurements from the buffer to the actual
   * focusedWemo pointer.
   *
   * The WEMO has been observed sending back -1's from time to time. If we
   * encounter one of these, we drop the entire measurement and move on to the
   * next one.
   */
  for(i = 0; i < wemomanager_size(); i++) {
#if 0
    if((focusedWemo = wemomanager_get(i)) != NULL) {
      if(focusedWemo->inUse && focusedWemo->override == 0) {
        SYSLOG_DEBUG("Capturing WEMO schedules at IP %s", focusedWemo->ip);

        // Cool schedule
        snprintf(url, sizeof(url), "http://%s/tstat/program/cool", focusedWemo->ip);
        if (libhttpcomm_getMsg(NULL, url, NULL, NULL, rxBuffer, sizeof(rxBuffer), params, NULL) == 0) {
          // We run the message through the JSON parser to make sure it's valid
          if (cJSON_Parse(rxBuffer) != NULL) {
            strcpy(focusedWemo->programCool, rxBuffer);
          }
        }

        // Heat schedule
        snprintf(url, sizeof(url), "http://%s/tstat/program/heat", focusedWemo->ip);
        if (libhttpcomm_getMsg(NULL, url, NULL, NULL, rxBuffer, sizeof(rxBuffer), params, NULL) == 0) {
          if (cJSON_Parse(rxBuffer) != NULL) {
            strcpy(focusedWemo->programHeat, rxBuffer);
          }
        }
      }
    }
#endif
  }
}


/**
 * Send measurements for all thermostats
 */
void wemomeasure_send() {
  char myMsg[PROXY_MAX_MSG_LEN];
  char buffer[10];
  int i;
  int offset = 0;
  wemo_t *focusedWemo;


  for(i = 0; i < wemomanager_size(); i++) {
    if((focusedWemo = wemomanager_get(i)) != NULL) {
#if 0
      if(focusedWemo->inUse && focusedWemo->measurementsUpdated) {
        focusedWemo->measurementsUpdated = false;

        offset = 0;

        // I didn't want to fill up our buffer here, so each thermostat gets
        // its own message and we toss it like a hot potato
        iotxml_newMsg(myMsg, sizeof(myMsg));

        snprintf(buffer, sizeof(buffer), "%4.2f", focusedWemo->temp);
        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedWemo->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "temp",
            NULL,
            0,
            buffer);

        snprintf(buffer, sizeof(buffer), "%3.1lf", focusedWemo->heat);
        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedWemo->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "targetTempHeat",
            NULL,
            0,
            buffer);

        snprintf(buffer, sizeof(buffer), "%3.1lf", focusedWemo->cool);
        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedWemo->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "targetTempCool",
            NULL,
            0,
            buffer);

        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedWemo->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "program/cool",
            NULL,
            0,
            focusedWemo->programCool);

        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedWemo->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "program/heat",
            NULL,
            0,
            focusedWemo->programHeat);

        offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
            focusedWemo->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "tmode",
            NULL,
            0,
            focusedWemo->tmode);

        offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
            focusedWemo->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "fmode",
            NULL,
            0,
            focusedWemo->fmode);

        offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
            focusedWemo->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "hold",
            NULL,
            0,
            focusedWemo->hold);

        offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
            focusedWemo->uuid,
            WEMO_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "override",
            NULL,
            0,
            focusedWemo->override);


        if(focusedWemo->tstate >= 0) {
          offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
              focusedWemo->uuid,
              WEMO_DEVICE_TYPE,
              IOT_PARAM_MEASURE,
              "tstate",
              NULL,
              0,
              focusedWemo->tstate);
        }

        if(focusedWemo->fstate >= 0) {
          offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
              focusedWemo->uuid,
              WEMO_DEVICE_TYPE,
              IOT_PARAM_MEASURE,
              "fstate",
              NULL,
              0,
              focusedWemo->fstate);
        }

        iotxml_send(myMsg, sizeof(myMsg));
      }
#endif
    }
  }
}


/***************** Private Functions *****************/


