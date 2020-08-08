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

#include "WiFiConfig.h"

const uint8_t WiFiConfigCount = 2;

class Configuration
{
public:
    Configuration(void);
    ~Configuration(void);

    // TODO: nodeName should be implemented
    uint32_t version;
    WiFiConfig wcfg[WiFiConfigCount];
    // TODO: Static IP not implemented !
    // TODO: There should be ipInfo for each WiFiConfig !
    tcpip_adapter_ip_info_t ipInfo;

    esp_err_t InitializeNVS(void);

    void InitData(void);

    esp_err_t ReadConfiguration(void);
    esp_err_t WriteConfiguration(bool eraseAll);

private:
};

extern Configuration configuration;

#endif
