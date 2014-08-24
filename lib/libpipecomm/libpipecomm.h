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

#ifndef LIBPIPECOMM_H
#define LIBPIPECOMM_H

#include <rpc/types.h>

/***************** Public Prototypes ****************/
int libpipecomm_open(const char* pipeName, bool_t isBlocking);

int libpipecomm_write(int fd, const char *msg, uint16_t msgLen);

int libpipecomm_read(int fd, char *msg, uint16_t maxLen);

#endif

