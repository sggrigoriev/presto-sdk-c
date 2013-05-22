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
 * Every message to the server must be wrapped in an XML header.  This
 * module is responsible for generating that header, which is put in place
 * before the message exits the local device through the proxy.
 *
 * @author David Moss
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "eui64.h"
#include "ioterror.h"
#include "iotdebug.h"

static uint32_t sequenceNum = 0;

/**
 * Wraps the message to the server inside an XML header and footer
 *
 * @param msg The message to wrap
 * @param maxSize The maximum size of the message buffer
 * @param sequenceNum Sequence number of the XML message
 * @return Number of bytes written or -1 for error
 */
int h2swrapper_wrap(char *dest, char *message, int destSize) {
  int bytesWritten = 0;
  char localAddress[EUI64_STRING_SIZE];

  assert(dest);
  assert(message);

  sequenceNum++;

  eui64_toString(localAddress, sizeof(localAddress));

  bytesWritten += snprintf(dest, destSize - bytesWritten,
      "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"
        "<h2s ver=\"2\" hubId=\"%s\" seq=\"%u\">", localAddress, sequenceNum);

  strncpy(dest + bytesWritten, message, destSize - bytesWritten);

  bytesWritten += strlen(message);

  bytesWritten += snprintf(dest + bytesWritten, destSize - bytesWritten, "</h2s>");

  return bytesWritten;
}

