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

#ifndef PROXYSERVER_H
#define PROXYSERVER_H

#ifndef DEFAULT_PROXY_PORT
#define DEFAULT_PROXY_PORT 60110
#endif

#ifndef DEFAULT_PROXY_URL
#define DEFAULT_PROXY_URL "developer.peoplepowerco.com:8080/deviceio/ml"
#endif

#ifndef DEFAULT_ACTIVATION_URL
#define DEFAULT_ACTIVATION_URL "developer.peoplepowerco.com/cloud/xml"
#endif

#ifndef DEFAULT_PROXY_DEVICETYPE
#define DEFAULT_PROXY_DEVICETYPE "4"
#endif

#ifndef DEFAULT_PROXY_CONFIG_FILENAME
#define DEFAULT_PROXY_CONFIG_FILENAME "proxy.conf"
#endif

/** Name of the token in our config file that stores the device type */
#define CONFIGIO_PROXY_DEVICE_TYPE_TOKEN_NAME "PROXY_DEVICE_TYPE"

/** Token for the path to the local proxy SSL certificate */
#define CONFIGIO_PROXY_SSL_CERTIFICATE_PATH "PROXY_SSL_CERTIFICATE_PATH"

/** Token to store the number of times this proxy app has been rebooted */
#define CONFIGIO_PROXY_REBOOTS "PROXY_REBOOTS"

/** Token for host address of the cloud, i.e. "developers.peoplepowerco.com" */
#define CONFIGIO_CLOUD_HOST "CLOUD_HOST"

/** Token for port address to the cloud, i.e "8080" */
#define CONFIGIO_CLOUD_PORT "CLOUD_PORT"

/** Token for URI of the cloud, i.e. "deviceio/ml" */
#define CONFIGIO_CLOUD_URI "CLOUD_URI"

/** Token for true or false to use SSL */
#define CONFIGIO_CLOUD_USE_SSL "CLOUD_USE_SSL"

/** Token to store the authentication key for the cloud */
#define CONFIGIO_CLOUD_ACTIVATION_KEY "CLOUD_AUTHENTICATION_KEY"

/** Name of the token in our config file that stores the activation url */
#define CONFIGIO_ACTIVATION_URL_TOKEN_NAME "APPLICATION_API_URL"

/** Name of the user that activated the proxy */
#define CONFIGIO_PROXY_ACTIVATION_USERNAME "USER_ACTIVATION_USERNAME"

#endif
