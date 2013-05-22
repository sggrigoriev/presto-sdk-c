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


#ifndef PROXYACTIVATION_H
#define PROXYACTIVATION_H

#include <limits.h>
#include <rpc/types.h>

#include "libhttpcomm.h"

enum {
  ACTIVATION_XML_TAG_SIZE = 32,
  ACTIVATION_XML_VALUE_SIZE = 64,
};

/**
 * Activation info to hold during XML parsing
 */
typedef struct activation_info_t {
      int resultCode;
      char xmlTag[ACTIVATION_XML_TAG_SIZE];
} activation_info_t;

/***************** Public Prototypes ****************/
error_t proxyactivation_activate(const char *activationKey, const char *optionalUsername);

#endif

