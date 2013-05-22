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
 * This module is responsible for tracking open file descriptors so we may
 * use them to broadcast to lister sockets
 * @author David Moss
 */

#include <stdbool.h>
#include <stdlib.h>

#include "proxyclientmanager.h"
#include "ioterror.h"
#include "iotdebug.h"

/** Array of clients, 1 element per open client socket */
proxy_client_t clients[PROXYCLIENTMANAGER_CLIENTS];


/**
 * Add a client socket
 * @param fd File descriptor to add
 * @return SUCCESS if the client was added
 */
error_t proxyclientmanager_add(int fd) {
  int i;
  SYSLOG_DEBUG("Add %d", fd);
  for(i = 0; i < PROXYCLIENTMANAGER_CLIENTS; i++) {
    if(!clients[i].inUse) {
      clients[i].inUse = true;
      clients[i].fd = fd;
      return SUCCESS;
    }
  }

  return FAIL;
}

/**
 * Remove the file descriptor from the list we're tracking
 * @param fd File descriptor
 */
void proxyclientmanager_remove(int fd) {
  int i;
  SYSLOG_DEBUG("Remove %d", fd);
  for(i = 0; i < PROXYCLIENTMANAGER_CLIENTS; i++) {
    if(clients[i].fd == fd) {
      clients[i].inUse = false;
    }
  }
}

/**
 * @return the number of elements in our proxy_client_t array
 */
int proxyclientmanager_size() {
  return PROXYCLIENTMANAGER_CLIENTS;
}

/**
 * @param index Index into the array of clients
 * @return the array of proxy_client_t's
 */
proxy_client_t *proxyclientmanager_get(int index) {
  if(index < PROXYCLIENTMANAGER_CLIENTS) {
    return &clients[index];
  } else {
    return NULL;
  }
}

