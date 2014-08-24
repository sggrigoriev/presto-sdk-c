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
 * Example XML message generation
 * @author David Moss
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ioterror.h"
#include "iotdebug.h"
#include "iotapi.h"

static int TOTAL_DEVICES = 2;

static int DEVICE_TYPE = 0;

/***************** Private Prototypes ****************/
void _xmlexample_msg();

/***************** Public Functions ****************/
int main() {
  _xmlexample_msg();
  return 0;
}

/*
 * After the XML generator is finished generating a message, it attempts to
 * send a message to this function.  The IOT XML generator does not have this
 * function implemented because the application layer is responsible for
 * routing the outgoing message to whatever communication mechanism we're using.
 *
 * In this case, we really don't care about sending the message, we just
 * print out the XML to the terminal.
 *
 * @param msg The constructed message to send
 * @param len The size of the message
 * @return SUCCESS if the message is sent
 */
error_t application_send(const char *msg, int len) {
  printf("\n\n%s", msg);
  return SUCCESS;
}

/***************** Private Functions *****************/
void _xmlexample_msg() {
  char myMsg[4096];
  char buffer[10];
  char deviceId[10];
  int i;
  int offset = 0;

  for (i = 0; i < TOTAL_DEVICES; i++) {

    // I didn't want to fill up our buffer here, so each emulated device
    // gets its own message.
    iotxml_newMsg(myMsg, sizeof(myMsg));
    offset = 0;

    // Create an emulated unique device ID for each device
    snprintf(deviceId, sizeof(deviceId), "%d", i);

    // We have to print the floats / doubles to a string in order
    // to format it as we want to see it at the server. That's why
    // we do an snprintf(..) to a buffer, and then add that buffer
    // to our message.
    //
    // Also, since my example gadget is a 1-socket smart outlet,
    // I use 0 in place of the index because there are not multiple
    // outlets on each example device.  If we had a multiple outlets,
    // I would have used the character '0', '1', '2', .. as the index
    // for each individual outlet.

    // Current
    snprintf(buffer, sizeof(buffer), "%4.2lf", 1.0);
    offset += iotxml_addString(
        myMsg + offset, // Offset into the existing message to start writing XML
        sizeof(myMsg) - offset,  // Amount of space we have left to write
        deviceId, // Emulated unique device ID. Each for-loop has a unique ID
        DEVICE_TYPE, // Device ID, given to us when we registered our device with ESP
        IOT_PARAM_MEASURE, // We are sending a measurement message
        "current", // The parameter name of this measurement
        "m", // The multiplier of the units of measurement
        0, // ASCII character to be used as an index in case of multiple outlets
        buffer); // Actual measurement to add, ours is a pre-formatted string

    // Power
    snprintf(buffer, sizeof(buffer), "%4.2lf", 2.0);
    offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
        deviceId, DEVICE_TYPE, IOT_PARAM_MEASURE, "power",
        "1", 0, buffer);

    // Voltage
    snprintf(buffer, sizeof(buffer), "%3.1lf", 3.0);
    offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
        deviceId, DEVICE_TYPE, IOT_PARAM_MEASURE, "volts",
        "1", 0, buffer);

    // Energy
    snprintf(buffer, sizeof(buffer), "%3.1lf", 4.0);
    offset += iotxml_addString(myMsg + offset, sizeof(myMsg) - offset,
        deviceId, DEVICE_TYPE, IOT_PARAM_MEASURE, "energy",
        "k", 0, buffer);

    // Power Factor
    offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
        deviceId, DEVICE_TYPE, IOT_PARAM_MEASURE,
        "powerFactor", NULL, 0, 5);

    // Outlet status
    offset += iotxml_addInt(myMsg + offset, sizeof(myMsg) - offset,
        deviceId, DEVICE_TYPE, IOT_PARAM_MEASURE,
        "outletStatus", NULL, 0, 6);

    // Send the measurement
    iotxml_send(myMsg, sizeof(myMsg));
  }
}

