/**
This file is part of pax-devices (https://github.com/CalinRadoni/pax-devices)
Copyright (C) 2019+ by Calin Radoni

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include <string.h>

#include "HTTPSrvLampD1.h"

// -----------------------------------------------------------------------------

static const char* TAG = "HTTPSrvLampD1";

// -----------------------------------------------------------------------------

HTTPSrvLampD1::HTTPSrvLampD1() : PaxHttpServer()
{
    //
}

HTTPSrvLampD1::~HTTPSrvLampD1(void)
{
    //
}