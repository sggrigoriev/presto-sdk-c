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


#ifndef IOTPARSER_H
#define IOTPARSER_H

/**
 * Maximum size of the value to expect from the server,
 * configurable at compile time
 */
#ifndef IOTPARSER_VALUE_SIZE
#define IOTPARSER_VALUE_SIZE 128
#endif

/** <s2h ..> tag */
#define IOTPARSER_TAG_S2H "s2h"

/** <param ..> tag */
#define IOTPARSER_TAG_PARAM "param"

/** <command ..> tag */
#define IOTPARSER_TAG_COMMAND "command"

/** cmdId attribute */
#define IOTPARSER_ATTR_COMMANDID "cmdId"

/** deviceId attribute */
#define IOTPARSER_ATTR_DEVICEID "deviceId"

/** name attribute */
#define IOTPARSER_ATTR_COMMANDNAME "name"

/** type attribute */
#define IOTPARSER_ATTR_COMMANDTYPE "type"

/** index attribute */
#define IOTPARSER_ATTR_INDEX "index"

#endif

