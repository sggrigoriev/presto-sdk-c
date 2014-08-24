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

#ifndef PROXYCONFIG_H
#define PROXYCONFIG_H

#include <stdbool.h>
#include "ioterror.h"

/** Default upload interval in seconds, can be overridden at compile time */
#ifndef PROXY_DEFAULT_UPLOAD_INTERVAL_SEC
#define PROXY_DEFAULT_UPLOAD_INTERVAL_SEC 60
#endif

enum {
  PROXY_URL_SIZE = 256,
  PROXY_MAX_HTTP_SEND_MESSAGE_LEN = 32768U,
  PROXY_MAX_ACTIVATION_TOKEN_SIZE = 128,
};

/***************** Public Prototypes ****************/
void proxyconfig_start();

void proxyconfig_stop();

long proxyconfig_getUploadIntervalSec();

void proxyconfig_setUploadIntervalSec(long uploadIntervalSec);

void proxyconfig_getUrl(char *dest, int destLen);

error_t proxyconfig_setUrl(const char *url);

const char *proxyconfig_getCertificate();

void proxyconfig_setCertificate(const char *certificate);

const char *proxyconfig_getActivationToken();

void proxyconfig_setActivationToken(const char *token);

void proxyconfig_setSsl(bool ssl);

bool proxyconfig_getSsl();


#endif
