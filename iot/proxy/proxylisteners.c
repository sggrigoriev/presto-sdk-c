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
 * This module allows the proxy to broadcast to all interested listeners
 * who want to receive commands from the server
 *
 * @author David Moss
 */

#include <pthread.h>
#include <errno.h>
#include <stdbool.h>

#include "proxylisteners.h"
#include "iotdebug.h"
#include "ioterror.h"

/** Array of listeners */
static struct {

  proxylistener l;

  bool inUse;

} proxyListeners[TOTAL_PROXY_LISTENERS];

/** Mutex to protect proxyListeners */
static pthread_mutex_t sProxyListenersMutex;


/***************** Proxylisteners Public ****************/
/**
 * Start the proxy listener by initialize the mutex
 */
void proxylisteners_start() {
  pthread_mutex_init(&sProxyListenersMutex, NULL);
}

/**
 * Stop the proxy listener by destroying the mutex
 */
void proxylisteners_stop() {
  pthread_mutex_destroy(&sProxyListenersMutex);
}


/**
 * Add a listener to the messages sent by the server.  This simply
 * forwards to the correct listener module.
 *
 * @param proxylistener Function pointer to a function(const char *msg, int len)
 * @return SUCCESS if the listener was added
 */
error_t proxylisteners_addListener(proxylistener l) {
  int i;

  pthread_mutex_lock(&sProxyListenersMutex);
  for(i = 0; i < TOTAL_PROXY_LISTENERS; i++) {
    if(proxyListeners[i].inUse && proxyListeners[i].l == l) {
      // Already in the log
      SYSLOG_DEBUG("Listener already exists");
      pthread_mutex_unlock(&sProxyListenersMutex);
      return SUCCESS;
    }
  }

  for(i = 0; i < TOTAL_PROXY_LISTENERS; i++) {
    if(!proxyListeners[i].inUse) {
      SYSLOG_DEBUG("Adding proxy listener to element %d", i);
      proxyListeners[i].inUse = true;
      proxyListeners[i].l = l;
      pthread_mutex_unlock(&sProxyListenersMutex);
      return SUCCESS;
    }
  }
  pthread_mutex_unlock(&sProxyListenersMutex);

  return FAIL;
}

/**
 * Remove a listener from the proxy
 * @param proxylistener Function pointer to remove
 * @return SUCCESS if the listener was found and removed
 */
error_t proxylisteners_removeListener(proxylistener l) {
  int i;

  pthread_mutex_lock(&sProxyListenersMutex);
  for(i = 0; i < TOTAL_PROXY_LISTENERS; i++) {
    if(proxyListeners[i].inUse && proxyListeners[i].l == l) {
      SYSLOG_DEBUG("Removing proxy listener at element %d", i);
      proxyListeners[i].inUse = false;
      pthread_mutex_unlock(&sProxyListenersMutex);
      return SUCCESS;
    }
  }
  pthread_mutex_unlock(&sProxyListenersMutex);

  return FAIL;
}

/**
 * Broadcast a message to all proxy listeners
 * @param msg Message to broadcast
 * @param len Length of the message
 */
error_t proxylisteners_broadcast(const char *msg, int len) {
  int i;

  if(*msg && len > 0) {
    SYSLOG_INFO("[broadcast]: %s", msg);

    pthread_mutex_lock(&sProxyListenersMutex);
    for(i = 0; i < TOTAL_PROXY_LISTENERS; i++) {
      if(proxyListeners[i].inUse) {
        SYSLOG_INFO("[broadcast]: Broadcasting to known client");
        proxyListeners[i].l(msg, len);
      }
    }
    pthread_mutex_unlock(&sProxyListenersMutex);

  } else {
    SYSLOG_INFO("[broadcast]: Nobody to broadcast to :(");
    return FAIL;
  }

  return SUCCESS;
}

/**
 * @return the total number of registered listeners
 */
int proxylisteners_totalListeners() {
  int i;
  int total = 0;

  pthread_mutex_lock(&sProxyListenersMutex);
  for(i = 0; i < TOTAL_PROXY_LISTENERS; i++) {
    if(proxyListeners[i].inUse) {
      total++;
    }
  }
  pthread_mutex_unlock(&sProxyListenersMutex);

  return total;
}
