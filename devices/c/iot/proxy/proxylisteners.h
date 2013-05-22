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

#ifndef PROXYLISTENERS_H
#define PROXYLISTENERS_H

#include "ioterror.h"

#ifndef __error_t_defined
#warning "error_t not defined"
#endif

/** Total listeners with a direct connection to the proxy instance */
#ifndef TOTAL_PROXY_LISTENERS
#define TOTAL_PROXY_LISTENERS 10
#endif

/** Proxy listener function pointer definition */
typedef void (*proxylistener)(const char *, int);

/***************** Public Prototypes ****************/
void proxylisteners_start();

void proxylisteners_stop();

error_t proxylisteners_addListener(proxylistener l);

error_t proxylisteners_removeListener(proxylistener l);

error_t proxylisteners_broadcast(const char *msg, int len);

int proxylisteners_totalListeners();

#endif
