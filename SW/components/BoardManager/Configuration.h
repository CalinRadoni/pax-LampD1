/**
This file is part of BoardManager esp-idf component from
pax-devices (https://github.com/CalinRadoni/pax-devices)
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

#ifndef Configuration_H
#define Configuration_H

#include "freertos/FreeRTOS.h"
#include "tcpip_adapter.h"

const uint8_t MaxNameLen = 64;
const uint8_t APConfigCnt = 3;

struct APConfig
{
    uint8_t SSID[32];
    uint8_t Pass[64];
};

class Configuration
{
public:
    Configuration(void);
    virtual ~Configuration();

    uint32_t version;
    char name[MaxNameLen];
    APConfig apCfg[APConfigCnt];
    tcpip_adapter_ip_info_t ipInfo;

    esp_err_t InitializeNVS(void);

    void InitData(void);

    esp_err_t ReadConfiguration(void);
    esp_err_t WriteConfiguration(bool eraseAll);

private:
};

extern Configuration configuration;

#endif
