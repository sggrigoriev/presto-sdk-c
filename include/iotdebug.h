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

#ifndef IOTDEBUG_H
#define IOTDEBUG_H

#include <syslog.h>

/** Dummy function for SYSLOG_* macros to use when we optimize out text */
static void __attribute__((unused)) dummySyslog(__const char *__fmt, ...) { }

#define SYSLOG_LEVEL_DEBUG 0
#define SYSLOG_LEVEL_INFO 1
#define SYSLOG_LEVEL_NOTICE 2
#define SYSLOG_LEVEL_WARNING 3
#define SYSLOG_LEVEL_ERR 4
#define SYSLOG_LEVEL_CRIT 5
#define SYSLOG_LEVEL_ALERT 6

#ifndef SYSLOG_LEVEL
#define SYSLOG_LEVEL SYSLOG_LEVEL_DEBUG
#endif


#if SYSLOG_LEVEL <= SYSLOG_LEVEL_ALERT
#define SYSLOG_ALERT(formatString, ...) syslog (LOG_ALERT, "%s(): "formatString, __FUNCTION__, ##__VA_ARGS__)
#else
#define SYSLOG_ALERT(formatString, ...) dummySyslog(""formatString, ##__VA_ARGS__);
#endif


#if SYSLOG_LEVEL <= SYSLOG_LEVEL_CRIT
#define SYSLOG_CRIT(formatString, ...) syslog (LOG_CRIT, "%s(): "formatString, __FUNCTION__, ##__VA_ARGS__)
#else
#define SYSLOG_CRIT(formatString, ...) dummySyslog(""formatString, ##__VA_ARGS__);
#endif

#if SYSLOG_LEVEL <= SYSLOG_LEVEL_ERR
#define SYSLOG_ERR(formatString, ...) syslog (LOG_ERR, "%s(): "formatString, __FUNCTION__, ##__VA_ARGS__)
#else
#define SYSLOG_ERR(formatString, ...) dummySyslog(""formatString, ##__VA_ARGS__);
#endif

#if SYSLOG_LEVEL <= SYSLOG_LEVEL_WARNING
#define SYSLOG_WARNING(formatString, ...) syslog (LOG_WARNING, "%s(): "formatString, __FUNCTION__, ##__VA_ARGS__)
#else
#define SYSLOG_WARNING(formatString, ...) dummySyslog(""formatString, ##__VA_ARGS__);
#endif

#if SYSLOG_LEVEL <= SYSLOG_LEVEL_NOTICE
#define SYSLOG_NOTICE(formatString, ...) syslog (LOG_NOTICE, "%s(): "formatString, __FUNCTION__, ##__VA_ARGS__)
#else
#define SYSLOG_NOTICE(formatString, ...) dummySyslog(""formatString, ##__VA_ARGS__);
#endif

#if SYSLOG_LEVEL <= SYSLOG_LEVEL_INFO
#define SYSLOG_INFO(formatString, ...) syslog (LOG_INFO, "%s(): "formatString, __FUNCTION__, ##__VA_ARGS__)
#else
#define SYSLOG_INFO(formatString, ...) dummySyslog(""formatString, ##__VA_ARGS__);
#endif

#if SYSLOG_LEVEL <= SYSLOG_LEVEL_DEBUG
#define SYSLOG_DEBUG(formatString, ...) syslog (LOG_DEBUG, "%s(): "formatString, __FUNCTION__, ##__VA_ARGS__)
#else
#define SYSLOG_DEBUG(formatString, ...) dummySyslog(""formatString, ##__VA_ARGS__);
#endif

#endif
