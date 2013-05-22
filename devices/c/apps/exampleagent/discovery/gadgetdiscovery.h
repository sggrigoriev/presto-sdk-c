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

#ifndef GADGETDISCOVERY_H
#define GADGETDISCOVERY_H

#define GADGET_LOCATION_HDR "Location:"

#define GADGET_SSDP_ADDR "239.255.255.250"

#define GADGET_SSDP_PORT 1900


#define GADGET_JSON_ATTR_MODEL "model"

#define GADGET_JSON_ATTR_UUID "uuid"

#define GADGET_JSON_ATTR_API_VERSION "api_version"

#define GADGET_JSON_ATTR_FW_VERSION "fw_version"

#define GADGET_JSON_ATTR_WLAN_FW_VERSION "wlan_fw_version"

/***************** Public Prototypes ****************/
error_t gadgetdiscovery_runOnce();


#endif
