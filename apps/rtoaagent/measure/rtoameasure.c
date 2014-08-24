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
 * This module takes measurements from all RTOA thermostats in our network
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
#include "rtoamanager.h"
#include "rtoameasure.h"
#include "rtoaagent.h"
#include "iotapi.h"


/***************** Private Prototypes ****************/

/***************** Public Functions ****************/
/**
 * Capture measurements for all known thermostats
 * We will drop all measurements that have a -1 in them, since that's an error
 */
void rtoameasure_capture() {
  int i;
  rtoa_t rtoaBuffer;
  rtoa_t *focusedRtoa;
  char url[PATH_MAX];
  char rxBuffer[RTOA_MAX_MSG_SIZE];
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
   * focusedRtoa pointer.
   *
   * The RTOA has been observed sending back -1's from time to time. If we
   * encounter one of these, we drop the entire measurement and move on to the
   * next one.
   */
  for(i = 0; i < rtoamanager_size(); i++) {
    if((focusedRtoa = rtoamanager_get(i)) != NULL) {
      if(focusedRtoa->inUse) {
        // If a value doesn't exist on the thermostat, it appears as -1 at the server
        memset(&rtoaBuffer, -1, sizeof(rtoaBuffer));

        snprintf(url, sizeof(url), "http://%s/tstat", focusedRtoa->ip);

        if (libhttpcomm_getMsg(NULL, url, NULL, NULL, rxBuffer, sizeof(rxBuffer), params, NULL) == 0) {
          SYSLOG_DEBUG("http://%s/tstat returned: %s", focusedRtoa->ip, rxBuffer);

          if ((jsonMsg = cJSON_Parse(rxBuffer)) != NULL) {

            // Temperature
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_TEMP)) != NULL) {
              if(jsonObject->valueint != -1) {
                rtoaBuffer.temp = jsonObject->valuedouble;
              } else {
                continue;
              }
            }

            // Tmode
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_TMODE)) != NULL) {
              if(jsonObject->valueint != -1) {
                rtoaBuffer.tmode = jsonObject->valueint;
              } else {
                continue;
              }
            }

            // Fmode
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_FMODE)) != NULL) {
              if(jsonObject->valueint != -1) {
                rtoaBuffer.fmode = jsonObject->valueint;
              } else {
                continue;
              }
            }

            // Hold
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_HOLD)) != NULL) {
              if(jsonObject->valueint != -1) {
                rtoaBuffer.hold = jsonObject->valueint;
              } else {
                continue;
              }
            }

            // Override
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_OVERRIDE)) != NULL) {
              if(jsonObject->valueint != -1) {
                rtoaBuffer.override = jsonObject->valueint;
              } else {
                continue;
              }
            }

            // Heat
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_HEAT)) != NULL) {
              if(jsonObject->valueint != -1) {
                rtoaBuffer.heat = jsonObject->valuedouble;
              } else {
                continue;
              }
            } else {
              // We aren't in heater mode
              rtoaBuffer.heat = 0;
            }

            // Cool
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_COOL)) != NULL) {
              if(jsonObject->valueint != -1) {
                rtoaBuffer.cool = jsonObject->valuedouble;
              } else {
                continue;
              }
            } else {
              // We aren't in cooler mode
              rtoaBuffer.cool = 0;
            }

            // Tstate
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_TSTATE)) != NULL) {
              if(jsonObject->valueint != -1) {
                rtoaBuffer.tstate = jsonObject->valueint;
              } else {
                continue;
              }
            }

            // Fstate
            if ((jsonObject = cJSON_GetObjectItem(jsonMsg, RTOA_JSON_ATTR_FSTATE)) != NULL) {
              if(jsonObject->valueint != -1) {
                rtoaBuffer.fstate = jsonObject->valueint;
              } else {
                continue;
              }
            }

            focusedRtoa->temp = rtoaBuffer.temp;
            focusedRtoa->tmode = rtoaBuffer.tmode;
            focusedRtoa->fmode = rtoaBuffer.fmode;
            focusedRtoa->hold = rtoaBuffer.hold;
            focusedRtoa->override = rtoaBuffer.override;
            focusedRtoa->heat = rtoaBuffer.heat;
            focusedRtoa->cool = rtoaBuffer.cool;
            focusedRtoa->tstate = rtoaBuffer.tstate;
            focusedRtoa->fstate = rtoaBuffer.fstate;
            focusedRtoa->measurementsUpdated = true;

            gettimeofday(&curTime, NULL);
            focusedRtoa->lastTouchTime.tv_sec = curTime.tv_sec;
          }
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
void rtoameasure_captureSchedules() {
  int i;
  rtoa_t *focusedRtoa;
  char url[PATH_MAX];
  char rxBuffer[RTOA_MAX_MSG_SIZE];
  http_param_t params;

  params.verbose = false;
  params.timeouts.connectTimeout = 3;
  params.timeouts.transferTimeout = 15;

  /*
   * We keep a buffer of measurements until we know all measurements are good.
   * Then at the end, we copy all the measurements from the buffer to the actual
   * focusedRtoa pointer.
   *
   * The RTOA has been observed sending back -1's from time to time. If we
   * encounter one of these, we drop the entire measurement and move on to the
   * next one.
   */
  for(i = 0; i < rtoamanager_size(); i++) {
    if((focusedRtoa = rtoamanager_get(i)) != NULL) {
      if(focusedRtoa->inUse && focusedRtoa->override == 0) {
        SYSLOG_DEBUG("Capturing RTOA schedules at IP %s", focusedRtoa->ip);

        // Cool schedule
        snprintf(url, sizeof(url), "http://%s/tstat/program/cool", focusedRtoa->ip);
        if (libhttpcomm_getMsg(NULL, url, NULL, NULL, rxBuffer, sizeof(rxBuffer), params, NULL) == 0) {
          // We run the message through the JSON parser to make sure it's valid
          if (cJSON_Parse(rxBuffer) != NULL) {
            strcpy(focusedRtoa->programCool, rxBuffer);
          }
        }

        // Heat schedule
        snprintf(url, sizeof(url), "http://%s/tstat/program/heat", focusedRtoa->ip);
        if (libhttpcomm_getMsg(NULL, url, NULL, NULL, rxBuffer, sizeof(rxBuffer), params, NULL) == 0) {
          if (cJSON_Parse(rxBuffer) != NULL) {
            strcpy(focusedRtoa->programHeat, rxBuffer);
          }
        }
      }
    }
  }
}


/**
 * Send measurements for all thermostats
 */
void rtoameasure_send() {
  char myMsg[PROXY_MAX_MSG_LEN];
  char buffer[10];
  int i;
  int offset = 0;
  rtoa_t *focusedRtoa;


  for(i = 0; i < rtoamanager_size(); i++) {
    if((focusedRtoa = rtoamanager_get(i)) != NULL) {
      if(focusedRtoa->inUse && focusedRtoa->measurementsUpdated) {
        focusedRtoa->measurementsUpdated = false;

        offset = 0;

        // I didn't want to fill up our buffer here, so each thermostat gets
        // its own message and we toss it like a hot potato
        iotxml_newMsg(myMsg, sizeof(myMsg));

        snprintf(buffer, sizeof(buffer), "%4.2f", focusedRtoa->temp);
        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            RTOA_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "temp",
            NULL,
            0,
            buffer);

        snprintf(buffer, sizeof(buffer), "%3.1lf", focusedRtoa->heat);
        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            RTOA_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "targetTempHeat",
            NULL,
            0,
            buffer);

        snprintf(buffer, sizeof(buffer), "%3.1lf", focusedRtoa->cool);
        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            RTOA_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "targetTempCool",
            NULL,
            0,
            buffer);

        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            RTOA_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "program/cool",
            NULL,
            0,
            focusedRtoa->programCool);

        offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            RTOA_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "program/heat",
            NULL,
            0,
            focusedRtoa->programHeat);

        offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            RTOA_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "tmode",
            NULL,
            0,
            focusedRtoa->tmode);

        offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            RTOA_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "fmode",
            NULL,
            0,
            focusedRtoa->fmode);

        offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            RTOA_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "hold",
            NULL,
            0,
            focusedRtoa->hold);

        offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
            focusedRtoa->uuid,
            RTOA_DEVICE_TYPE,
            IOT_PARAM_MEASURE,
            "override",
            NULL,
            0,
            focusedRtoa->override);


        if(focusedRtoa->tstate >= 0) {
          offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
              focusedRtoa->uuid,
              RTOA_DEVICE_TYPE,
              IOT_PARAM_MEASURE,
              "tstate",
              NULL,
              0,
              focusedRtoa->tstate);
        }

        if(focusedRtoa->fstate >= 0) {
          offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
              focusedRtoa->uuid,
              RTOA_DEVICE_TYPE,
              IOT_PARAM_MEASURE,
              "fstate",
              NULL,
              0,
              focusedRtoa->fstate);
        }

        iotxml_send(myMsg, sizeof(myMsg));
      }
    }
  }
}


/***************** Private Functions *****************/


