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

#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <stdbool.h>
#include "ioterror.h"

enum {
  CLIENTSOCKET_INBOUND_MSGSIZE = 4096,
};

/** The developer must implement this function in the application */
void application_receive(const char *msg, int len);

/***************** Public Prototypes ****************/
error_t clientsocket_open(const char *serverName, int port);

error_t clientsocket_close();

error_t clientsocket_send(const char *msg, int len);


#endif

