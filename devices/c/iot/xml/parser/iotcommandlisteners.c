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
 * This module tracks listener functions that want to receive commands from
 * the server.
 * @author David Moss
 */

#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "iotapi.h"
#include "iotdebug.h"
#include "ioterror.h"
#include "iotcommandlisteners.h"

/** Array of listeners */
static struct {

  commandlistener_f l;

  char type[TYPE_ATTRIBUTE_CHARS_TO_MATCH];

  bool inUse;

} listeners[TOTAL_COMMAND_LISTENERS];


/***************** Public Functions ****************/
/**
 * Add a listener to receive parsed commands
 *
 * @param commandlistener_f Function pointer to a function(command_t cmd)
 * @param type Type attribute to listen for, "set", "delete", "discover", etc.
 * @return SUCCESS if the listener was added
 */
error_t iotxml_addCommandListener(commandlistener_f l, char *type) {
  int i;

  for(i = 0; i < TOTAL_COMMAND_LISTENERS; i++) {
    if(listeners[i].inUse && listeners[i].l == l) {
      // Already in the log
      return SUCCESS;
    }
  }

  for(i = 0; i < TOTAL_COMMAND_LISTENERS; i++) {
    if(!listeners[i].inUse) {
      listeners[i].inUse = true;
      strncpy(listeners[i].type, type, TYPE_ATTRIBUTE_CHARS_TO_MATCH);
      listeners[i].l = l;
      return SUCCESS;
    }
  }

  return FAIL;
}

/**
 * Remove a listener from the command
 * @param commandlistener_f Function pointer to remove
 * @return SUCCESS if the listener was found and removed
 */
error_t iotxml_removeCommandListener(commandlistener_f l) {
  int i;

  for(i = 0; i < TOTAL_COMMAND_LISTENERS; i++) {
    if(listeners[i].inUse && listeners[i].l == l) {
      listeners[i].inUse = false;
      return SUCCESS;
    }
  }

  return FAIL;
}

/**
 * Broadcast a command to all listeners
 * @param msg Message to broadcast
 * @param len Length of the message
 * @param type Type attribute of the command describing the action
 */
error_t iotcommandlisteners_broadcast(command_t *cmd) {
  int i;

  for(i = 0; i < TOTAL_COMMAND_LISTENERS; i++) {
    if(listeners[i].inUse) {
      if(strncmp(listeners[i].type, cmd->commandType, TYPE_ATTRIBUTE_CHARS_TO_MATCH) == 0) {
        listeners[i].l(cmd);
      }
    }
  }

  return SUCCESS;
}

/**
 * @return the total number of registered listeners
 */
int iotcommandlisteners_totalListeners() {
  int i;
  int total = 0;

  for(i = 0; i < TOTAL_COMMAND_LISTENERS; i++) {
    if(listeners[i].inUse) {
      total++;
    }
  }

  return total;
}

