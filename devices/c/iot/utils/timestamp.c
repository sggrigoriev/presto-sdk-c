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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "timestamp.h"
#include "ioterror.h"
#include "iotdebug.h"


/**
 * Produces a timestamp in the format YYYY-MM-DDTHH:MM:SS[Z|[+|-]hh:mm]
 * @param dest Destination to write the timestamp string
 * @param maxSize Maximum size of the destination, at least TIMESTAMP_STAMP_SIZE
 * @return The size of the timestamp string
 */
int getTimestamp(char *dest, int maxSize) {
  time_t epochTime = time(NULL);
  struct tm *currentTime;
  int offset = 0;

  if(maxSize < TIMESTAMP_STAMP_SIZE) {
    return FAIL;
  }

  currentTime = localtime(&epochTime);
  offset = strftime(dest, maxSize, "%Y-%m-%dT%H:%M:%S", currentTime);
  getTimezone(dest + offset, maxSize - offset);

  return strlen(dest);
}

/**
 * Produces the time zone as specified by xsd:dateTime
 * @param dest Destination buffer to write the timezone into
 * @param size Size of the buffer, at least TIMESTAMP_ZONE_SIZE large
 */
void getTimezone(char *dest, int size) {
  struct tm *currentTime;
  time_t currtime;
  currtime = time(NULL);

  currentTime = localtime(&currtime);
  strftime(dest, TIMESTAMP_ZONE_SIZE, "%z", currentTime);

  //formatting from ISO 8601:2000 to Chapter 5.4 of ISO 8601
  // [+|-]hhmm will become [+|-]hh:mm
  dest[6] = 0;
  dest[5] = dest[4];
  dest[4] = dest[3];
  dest[3] = ':';
}
