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

#ifndef PROXYCLIENTMANAGER_H
#define PROXYCLIENTMANAGER_H

#include <stdbool.h>

#include "ioterror.h"

#ifndef PROXYCLIENTMANAGER_CLIENTS
#define PROXYCLIENTMANAGER_CLIENTS 30
#endif

/** Definition of a proxy client to track active listener sockets */
typedef struct proxy_client_t {

  /** File descriptor */
  int fd;

  /** True if this element is in use */
  bool inUse;

} proxy_client_t;

/***************** Public Prototypes *****************/
error_t proxyclientmanager_add(int fd);

void proxyclientmanager_remove(int fd);

int proxyclientmanager_size();

proxy_client_t *proxyclientmanager_get(int i);

#endif
