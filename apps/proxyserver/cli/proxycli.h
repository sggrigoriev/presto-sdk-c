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

#ifndef PROXYCLI_H
#define PROXYCLI_H

enum {
  PROXYCLI_CONFIG_FILE_PATH_SIZE = 40,
  PROXYCLI_CONFIG_CLOUD_NAME_SIZE = 16,
};

/***************** Public Prototypes ****************/
void proxycli_parse(int argc, char *argv[]);

int proxycli_getPort();

const char *proxycli_getActivationKey();

const char *proxycli_getConfigFilename();

const char *proxycli_getUsername();

const char *proxycli_getPassword();

const bool proxycli_readyToActivate();

const char *proxycli_setCloudName( const char * );

const char *proxycli_setDataFormat( const char * );

const char *proxycli_getCloudName();

const char *proxycli_getDataFormat();

#endif

