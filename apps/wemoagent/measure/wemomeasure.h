/*
 * Copyright (c) 2011 People Power Company
 * All rights reserved.
 *
 * This open source code was developed with funding from People Power Company
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the People Power Corporation nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * PEOPLE POWER CO. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE
 */



#ifndef WEMOMEASURE_H
#define WEMOMEASURE_H

#define WEMO_JSON_ATTR_TEMP "temp"

#define WEMO_JSON_ATTR_TMODE "tmode"

#define WEMO_JSON_ATTR_FMODE "fmode"

#define WEMO_JSON_ATTR_OVERRIDE "override"

#define WEMO_JSON_ATTR_HOLD "hold"

#define WEMO_JSON_ATTR_COOL "t_cool"

#define WEMO_JSON_ATTR_HEAT "t_heat"

#define WEMO_JSON_ATTR_TSTATE "tstate"

#define WEMO_JSON_ATTR_FSTATE "fstate"

#define WEMO_JSON_ATTR_TIME "time"

#define WEMO_JSON_ATTR_DAY "day"

#define WEMO_JSON_ATTR_HOUR "hour"

#define WEMO_JSON_ATTR_MINUTE "minute"


/***************** Public Prototypes ****************/
void wemomeasure_capture();

void wemomeasure_captureSchedules();

void wemomeasure_send();

#endif

