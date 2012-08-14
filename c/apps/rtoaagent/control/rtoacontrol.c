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
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

#include <cJSON.h>

#include "libhttpcomm.h"

#include "ioterror.h"
#include "iotdebug.h"
#include "proxy.h"
#include "rtoaagent.h"
#include "rtoamanager.h"
#include "rtoameasure.h"
#include "iotapi.h"

/** The last command ID we executed */
static int lastCommandId = -1;

/***************** Public Functions ****************/
/**
 * Besides the main loop, this is the only function to access the mutex.
 * We don't want to send a command to change the state of the thermostat
 * while in the middle of the main loop, otherwise the side effect could be
 * that we set the temperature and immediately touch the schedule which
 * will undo the temperature command.
 */
void rtoacontrol_execute(command_t *cmd) {
  rtoa_t *focusedRtoa;
  char jsonText[RTOA_MAX_MSG_SIZE];
  char rxBuffer[RTOA_MAX_MSG_SIZE];
  http_param_t params;
  char url[PATH_MAX];

  params.verbose = TRUE;
  params.timeouts.connectTimeout = 3;
  params.timeouts.transferTimeout = 10;

  if((focusedRtoa = rtoamanager_getByUuid(cmd->deviceId)) != NULL) {
    cJSON *jsonCmd;

    // dmm: Adding this in because the server keeps sending commands
    // for command ID's we've previously acknowledged and executed.
    if((lastCommandId >= 0) && (cmd->commandId <= lastCommandId) && (cmd->commandId != 0)) {
      SYSLOG_INFO("IGNORING DUPLICATE COMMAND ID: %d", cmd->commandId);
      iotxml_sendResult(cmd->commandId, IOT_RESULT_HUBERROR);
      return;

    } else {
      lastCommandId = cmd->commandId;
    }

    // tmode / fmode commands are integers
    if(strcmp(cmd->commandName, RTOA_JSON_ATTR_TMODE) == 0
        || strcmp(cmd->commandName, RTOA_JSON_ATTR_FMODE) == 0) {
      if(cmd->argument == NULL) {
        SYSLOG_INFO("[rtoa] Malformed command");
        iotxml_sendResult(cmd->commandId, IOT_RESULT_WRONGFORMAT);
        return;
      }

      jsonCmd = cJSON_CreateObject();
      cJSON_AddNumberToObject(jsonCmd, cmd->commandName, atoi(cmd->argument));
      strncpy(jsonText, cJSON_Print(jsonCmd), sizeof(jsonText));
      cJSON_Delete(jsonCmd);

      snprintf(url, sizeof(url), "%s/tstat", focusedRtoa->ip);
      SYSLOG_DEBUG("RTOA command: %s/tstat %s", focusedRtoa->ip, jsonText);

      pthread_mutex_lock(rtoaagent_getMutex());
      if(0 != libhttpcomm_sendMsg(NULL, CURLOPT_POST, url, NULL, NULL, jsonText, strlen(jsonText), rxBuffer, sizeof(rxBuffer), params, NULL)) {
        iotxml_sendResult(cmd->commandId, IOT_RESULT_DEVICECONNECTIONERROR);
        pthread_mutex_unlock(rtoaagent_getMutex());
        return;
      }
      pthread_mutex_unlock(rtoaagent_getMutex());

      if(strstr(rxBuffer, "success") == NULL) {
        iotxml_sendResult(cmd->commandId, IOT_RESULT_DEVICEEXECUTIONERROR);
        return;
      }

      iotxml_sendResult(cmd->commandId, IOT_RESULT_EXECUTED);
      rtoaagent_refreshDevices();
    }

    // t_heat is a double.  The server calls it "targetTempHeat"
    if(strcmp(cmd->commandName, RTOA_TARGET_TEMP_HEAT) == 0) {
      if(cmd->argument == NULL) {
        SYSLOG_INFO("[rtoa] Malformed command");
        iotxml_sendResult(cmd->commandId, IOT_RESULT_WRONGFORMAT);
        return;
      }

      jsonCmd = cJSON_CreateObject();
      cJSON_AddNumberToObject(jsonCmd, RTOA_JSON_ATTR_HEAT, atof(cmd->argument));
      strncpy(jsonText, cJSON_Print(jsonCmd), sizeof(jsonText));
      cJSON_Delete(jsonCmd);

      snprintf(url, sizeof(url), "%s/tstat", focusedRtoa->ip);
      SYSLOG_DEBUG("RTOA command: %s/tstat %s", focusedRtoa->ip, jsonText);

      pthread_mutex_lock(rtoaagent_getMutex());
      if(0 != libhttpcomm_sendMsg(NULL, CURLOPT_POST, url, NULL, NULL, jsonText, strlen(jsonText), rxBuffer, sizeof(rxBuffer), params, NULL)) {
        iotxml_sendResult(cmd->commandId, IOT_RESULT_DEVICECONNECTIONERROR);
        pthread_mutex_unlock(rtoaagent_getMutex());
        return;
      }
      pthread_mutex_unlock(rtoaagent_getMutex());

      if(strstr(rxBuffer, "success") == NULL) {
        iotxml_sendResult(cmd->commandId, IOT_RESULT_DEVICEEXECUTIONERROR);
        return;
      }

      iotxml_sendResult(cmd->commandId, IOT_RESULT_EXECUTED);
      rtoaagent_refreshDevices();
    }

    // t_cool is a double.  The server calls it "targetTempCool"
    if(strcmp(cmd->commandName, RTOA_TARGET_TEMP_COOL) == 0) {
      if(cmd->argument == NULL) {
        SYSLOG_INFO("[rtoa] Malformed command");
        iotxml_sendResult(cmd->commandId, IOT_RESULT_WRONGFORMAT);
        return;
      }

      jsonCmd = cJSON_CreateObject();
      cJSON_AddNumberToObject(jsonCmd, RTOA_JSON_ATTR_COOL, atof(cmd->argument));
      strncpy(jsonText, cJSON_Print(jsonCmd), sizeof(jsonText));
      cJSON_Delete(jsonCmd);

      snprintf(url, sizeof(url), "%s/tstat", focusedRtoa->ip);
      SYSLOG_DEBUG("RTOA command: %s/tstat %s", focusedRtoa->ip, jsonText);

      pthread_mutex_lock(rtoaagent_getMutex());
      if(0 != libhttpcomm_sendMsg(NULL, CURLOPT_POST, url, NULL, NULL, jsonText, strlen(jsonText), rxBuffer, sizeof(rxBuffer), params, NULL)) {
        iotxml_sendResult(cmd->commandId, IOT_RESULT_DEVICECONNECTIONERROR);
        pthread_mutex_unlock(rtoaagent_getMutex());
        return;
      }
      pthread_mutex_unlock(rtoaagent_getMutex());

      if(strstr(rxBuffer, "success") == NULL) {
        iotxml_sendResult(cmd->commandId, IOT_RESULT_DEVICEEXECUTIONERROR);
        return;
      }

      iotxml_sendResult(cmd->commandId, IOT_RESULT_EXECUTED);
      rtoaagent_refreshDevices();
    }

    // program/cool and program/heat are special JSON strings that get forwarded
    if(strstr(cmd->commandName, "program/heat") != NULL
        || strstr(cmd->commandName, "program/cool") != NULL) {
      char argument[cmd->argSize + 1];
      memset(argument, 0x0, sizeof(argument));

      if(cmd->argument == NULL) {
        SYSLOG_INFO("[rtoa] Malformed command");
        iotxml_sendResult(cmd->commandId, IOT_RESULT_WRONGFORMAT);
        return;
      }

      strncpy(argument, cmd->argument, cmd->argSize);

      snprintf(url, sizeof(url), "%s/tstat/%s", focusedRtoa->ip, cmd->commandName);
      SYSLOG_DEBUG("RTOA command: %s/tstat/%s", focusedRtoa->ip, cmd->commandName);

      pthread_mutex_lock(rtoaagent_getMutex());
      if(0 != libhttpcomm_sendMsg(NULL, CURLOPT_POST, url, NULL, NULL, argument, strlen(argument), rxBuffer, sizeof(rxBuffer), params, NULL)) {
        iotxml_sendResult(cmd->commandId, IOT_RESULT_DEVICECONNECTIONERROR);
        pthread_mutex_unlock(rtoaagent_getMutex());
        return;
      }
      pthread_mutex_unlock(rtoaagent_getMutex());

      if(strstr(rxBuffer, "success") == NULL) {
        iotxml_sendResult(cmd->commandId, IOT_RESULT_DEVICEEXECUTIONERROR);
        return;
      }

      iotxml_sendResult(cmd->commandId, IOT_RESULT_EXECUTED);
      rtoaagent_refreshDevices();
    }
  }
}

/**
 * Receive commands of type "discover" to discover new devices now.
 */
void rtoacontrol_discover(command_t *cmd) {
  rtoaagent_discover();
}

/**
 * Synchronize the time on the thermostat with the local machine,
 * which we assume has its time synchronized to a valid NTP server and
 * the timezone set properly.
 */
void rtoacontrol_synchronizeTimes() {
  int i;
  time_t rawtime;
  struct tm *now;
  cJSON *jsonMsg;
  cJSON *timeJsonObject;
  rtoa_t *focusedRtoa;
  char url[PATH_MAX];
  char txBuffer[RTOA_MAX_MSG_SIZE];
  char rxBuffer[RTOA_MAX_MSG_SIZE];
  http_param_t params;

  params.verbose = false;
  params.timeouts.connectTimeout = 3;
  params.timeouts.transferTimeout = 15;

  rawtime = time(NULL);
  now = localtime(&rawtime);

  jsonMsg = cJSON_CreateObject();
  timeJsonObject = cJSON_CreateObject();
  cJSON_AddItemToObject(jsonMsg, "time", timeJsonObject);
  cJSON_AddNumberToObject(timeJsonObject, "day", now->tm_wday - 1);
  cJSON_AddNumberToObject(timeJsonObject, "hour", now->tm_hour);
  cJSON_AddNumberToObject(timeJsonObject, "minute", now->tm_min);

  strncpy(txBuffer, cJSON_Print(jsonMsg), sizeof(txBuffer));

  for (i = 0; i < rtoamanager_size(); i++) {
    if ((focusedRtoa = rtoamanager_get(i)) != NULL) {

      // Only set the time when the thermostat is not in override mode,
      // or else you'll override the user's override and go back
      // to executing on a schedule.
      if(focusedRtoa->inUse && focusedRtoa->override == 0) {
        SYSLOG_INFO("[rtoa] Synchronizing time at %s", focusedRtoa->ip);
        snprintf(url, sizeof(url), "%s/tstat", focusedRtoa->ip);
        SYSLOG_DEBUG("RTOA Command: %s/tstat %s", focusedRtoa->ip, txBuffer);
        libhttpcomm_sendMsg(NULL, CURLOPT_POST, url, NULL, NULL, txBuffer, strlen(txBuffer), rxBuffer, sizeof(rxBuffer), params, NULL);
      }
    }
  }

  cJSON_Delete(jsonMsg);
}
