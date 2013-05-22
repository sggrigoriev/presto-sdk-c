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

#ifndef IOTCOMMANDLISTENERS_H
#define IOTCOMMANDLISTENERS_H

#include "iotapi.h"

/** Total listeners with a direct connection to the command parser */
#ifndef TOTAL_COMMAND_LISTENERS
#define TOTAL_COMMAND_LISTENERS 10
#endif

/** Number of characters to match in the 'type' attribute of a command */
#ifndef TYPE_ATTRIBUTE_CHARS_TO_MATCH
#define TYPE_ATTRIBUTE_CHARS_TO_MATCH 3
#endif

/***************** Public Prototypes ****************/
error_t iotcommandlisteners_broadcast(command_t *cmd);

int iotcommandlisteners_totalListeners();

#endif

