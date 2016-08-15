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
 * Radio Thermostat of America Agent
 *
 * @author David Moss
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>

#include "ioterror.h"
#include "iotdebug.h"
#include "proxyserver.h"
#include "clientsocket.h"
#include "iotapi.h"

#include "wemoagent.h"

/** Set to true to terminate all threads */
bool gTerminate;

/** Configurable heartbeat period */
static int heartbeatPeriod_sec = WEMO_HEARTBEAT_PERIOD_SEC;

/** Configurable measurement period */
static int measurementPeriod_sec = WEMO_MEASUREMENT_PERIOD_SEC;

/** Last discovery time */
static struct timeval lastDiscoveryTime;

/** Last heartbeat time */
static struct timeval lastHeartbeatTime;

/** Last measurement time */
static struct timeval lastMeasurementTime;

/** Last time the program schedule was captured */
static struct timeval lastProgramScheduleTime;

/** Last time the thermostats had their clocks synchronized */
static struct timeval lastTimeSyncTime;

/** Mutex to access thermostats */
static pthread_mutex_t thermostatMutex;

/***************** Functions ****************/
/**
 * Main function
 */
int main(int argc, char *argv[]) {
  SYSLOG_INFO("*************** WEMO Agent ***************");

  pthread_mutex_init(wemoagent_getMutex(), NULL);

  // Open a socket to the proxy server
  while(clientsocket_open("127.0.0.1", DEFAULT_PROXY_PORT) != SUCCESS) {
    SYSLOG_DEBUG("[wemo] Couldn't open client socket");
    sleep(5);
  }

  // Listen for commands
  iotxml_addCommandListener(&wemocontrol_execute, "set");
  iotxml_addCommandListener(&wemoagent_discover, "discover");

  printf("Radio Thermostat of America Agent running\n");
  printf("Monitor the syslogs (/var/log/messages) for runtime information\n");

  // Now we use i to keep track of time in each loop
  while(!gTerminate) {
    struct timeval curTime = { 0, 0 };

    while (!gTerminate) {
      pthread_mutex_lock(wemoagent_getMutex());

      gettimeofday(&curTime, NULL);

      // Discover devices periodically
      if ((curTime.tv_sec - lastDiscoveryTime.tv_sec) >= WEMO_DISCOVERY_PERIOD_SEC) {
        SYSLOG_INFO("[wemo] Attempting discovery");
        lastDiscoveryTime.tv_sec = curTime.tv_sec;
        wemodiscovery_runOnce();
      }

      // Send out heartbeats for our known devices periodically
      if ((curTime.tv_sec - lastHeartbeatTime.tv_sec) >= heartbeatPeriod_sec) {
        SYSLOG_INFO("[wemo] Heartbeat");
        lastHeartbeatTime.tv_sec = curTime.tv_sec;
        wemoheartbeat_send();
      }

      // Take measurements and kill off stragglers periodically
      if ((curTime.tv_sec - lastMeasurementTime.tv_sec) >= measurementPeriod_sec) {
        SYSLOG_INFO("[wemo] Measure");
        lastMeasurementTime.tv_sec = curTime.tv_sec;
        wemomeasure_capture();

        // Always grab the schedule after the latest measurement update
        // to make sure the schedule won't be grabbed while the thermostat
        // is in override mode
        if ((curTime.tv_sec - lastProgramScheduleTime.tv_sec) >= WEMO_CAPTURE_PROGRAM_PERIOD_SEC) {
          SYSLOG_INFO("[wemo] Capture schedules");
          lastProgramScheduleTime.tv_sec = curTime.tv_sec;
          wemomeasure_captureSchedules();
        }

        // Synchronize the thermostat's time periodically, but only do so
        // after grabbing the last measurement to make sure the thermostat
        // is not in override mode (or else you'll start executing the schedule)
        if ((curTime.tv_sec - lastTimeSyncTime.tv_sec) >= WEMO_TIME_SYNC_PERIOD_SEC) {
          SYSLOG_INFO("[wemo] Time sync");
          lastTimeSyncTime.tv_sec = curTime.tv_sec;
          wemocontrol_synchronizeTimes();
        }

        wemomanager_garbageCollection();
        wemomeasure_send();
      }

      pthread_mutex_unlock(wemoagent_getMutex());

      sleep(10);
    }
  }

  pthread_mutex_destroy(wemoagent_getMutex());

  return 0;
}

/**
 * The application layer is responsible for routing raw messages received
 * from the server.
 *
 * Because we use a clientsocket module to communicate with the proxy server,
 * the clientsocket module will execute this function upon receiving a message.
 * Here in our application layer, we route the message to the appropriate
 * parser. Currently this parser is XML, but in the future it could be JSON.
 * @param msg The received message
 * @param len The length of the message
 */
void application_receive(const char *msg, int len) {
  if(strstr(msg, "xml") != NULL) {
       iotxml_parse(msg, len);

    } else {
      SYSLOG_INFO("[client] Unknown message format: %s", msg);
    }
}

/**
 * The application layer is responsible for routing raw send messages to the
 * appropriate destination.  Because this client uses a clientsocket
 * module to send a message to the proxy server, we simply forward the
 * message on to the clientsocket module.
 * @param msg The constructed message to send
 * @param len The size of the message
 * @return SUCCESS if the message is sent
 */
error_t application_send(const char *msg, int len) {
  SYSLOG_DEBUG("[wemo] Send message: %s", msg);
  return clientsocket_send(msg, len);
}

/**
 * Set the measurement period
 * @param seconds Seconds between measurements
 */
void wemoagent_setMeasurementPeriod(int seconds) {
  measurementPeriod_sec = seconds;
}

/**
 * Set the heartbeat period
 * @param seconds Seconds between measurements
 */
void wemoagent_setHeartbeatPeriod(int seconds) {
  heartbeatPeriod_sec = seconds;
}

/**
 * When we have a new device added to the system, get its measurements and
 * capture its schedule
 */
void wemoagent_refreshDevices() {
  lastHeartbeatTime.tv_sec = 0;
  lastMeasurementTime.tv_sec = 0;
}

/**
 * Discover new devices
 */
void wemoagent_discover() {
  lastDiscoveryTime.tv_sec = 0;
}

/**
 * @return the mutex for accessing thermostats
 */
pthread_mutex_t *wemoagent_getMutex() {
  return &thermostatMutex;
}
