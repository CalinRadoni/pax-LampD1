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

#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"

#include <string.h>

#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "Configuration.h"

// -----------------------------------------------------------------------------

Configuration configuration;

static const char* TAG = "Configuration";

const uint32_t ConfigurationVersion = 1;
const char* ConfigNVS = "DevXX-Config";

// -----------------------------------------------------------------------------

Configuration::Configuration(void)
{
    InitData();
}

Configuration::~Configuration(void)
{
    //
}

esp_err_t Configuration::InitializeNVS(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        err = nvs_flash_erase();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%d nvs_flash_erase", err);
        }
        else {
            err = nvs_flash_init();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%d nvs_flash_init", err);
            }
        }
    }
    return err;
}

void Configuration::InitData(void)
{
    version = ConfigurationVersion;

    for (uint8_t i = 0; i < WiFiConfigCount; i++) {
        wcfg[i].Initialize();
    }

    ipInfo.ip.addr = 0;
    ipInfo.netmask.addr = 0;
    ipInfo.gw.addr = 0;
}

esp_err_t Configuration::ReadConfiguration(void)
{
    nvs_handle_t nvsHandle;
    size_t reqSize, size;

    InitData();

    esp_err_t err = nvs_open(ConfigNVS, NVS_READONLY, &nvsHandle);
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        // The namespace does not exists yet
        // Write a default configuration, no erase needed
        nvs_close(nvsHandle);
        ESP_LOGW(TAG, "%d nvs_open. Creating the namespaceand default config.", err);
        return WriteConfiguration(false);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%d nvs_open NVS_READONLY", err);
        return err;
    }

    err = nvs_get_u32(nvsHandle, "Version", &version);
    if (err != ESP_OK) {
        // Failed to read the configuration version from NVS
        // Write a default configuration and erase the previous storage
        nvs_close(nvsHandle);
        return WriteConfiguration(true);
    }

    if (version != ConfigurationVersion) {
        // Readed configuration version from NVS is different
        // Write a default configuration and erase the previous storage
        nvs_close(nvsHandle);
        return WriteConfiguration(true);
    }

#if WiFiConfigCount > 15
#error "WiFiConfigCount MUST be smaller then 16 or you must create another naming scheme !"
#endif

    for (uint8_t i = 0; i < WiFiConfigCount; i++) {
        char keyName[6];

        snprintf(keyName, 6, "SSID%x", i);
        size = reqSize = 32 * sizeof(uint8_t);
        err = nvs_get_blob(nvsHandle, keyName, (void*)wcfg[i].SSID, &size);
        if (err != ESP_OK || size != reqSize) {
            InitData();
            nvs_close(nvsHandle);
            return err;
        }

        snprintf(keyName, 6, "PASS%x", i);
        size = reqSize = 64 * sizeof(uint8_t);
        err = nvs_get_blob(nvsHandle, keyName, (void*)wcfg[i].PASS, &size);
        if (err != ESP_OK || size != reqSize) {
            InitData();
            nvs_close(nvsHandle);
            return err;
        }
    }

    size = reqSize = sizeof(tcpip_adapter_ip_info_t);
    err = nvs_get_blob(nvsHandle, "IPv4", (void*)&ipInfo, &size);
    if (err != ESP_OK || size != reqSize) {
        InitData();
        nvs_close(nvsHandle);
        return err;
    }

    nvs_close(nvsHandle);
    return err;
}

esp_err_t Configuration::WriteConfiguration(bool eraseAll)
{
    nvs_handle_t nvsHandle;
    size_t size;

    esp_err_t err = nvs_open(ConfigNVS, NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%d Failed to open NVS in readwrite mode", err);
        return err;
    }

    if (eraseAll) {
        err = nvs_erase_all(nvsHandle);
        if (err != ESP_OK) {
            nvs_close(nvsHandle);
            ESP_LOGE(TAG, "%d Failed to erase", err);
            return err;
        }
        err = nvs_commit(nvsHandle);
        if (err != ESP_OK) {
            nvs_close(nvsHandle);
            ESP_LOGE(TAG, "%d Failed to commit", err);
            return err;
        }
    }

    err = nvs_set_u32(nvsHandle, "Version", ConfigurationVersion);
    if (err != ESP_OK) {
        nvs_close(nvsHandle);
        return err;
    }
    err = nvs_commit(nvsHandle);
    if (err != ESP_OK) {
        nvs_close(nvsHandle);
        return err;
    }

#if WiFiConfigCount > 15
#error "WiFiConfigCount MUST be smaller then 16 or you must create another naming scheme !"
#endif

    for (uint8_t i = 0; i < WiFiConfigCount; i++) {
        char keyName[6];

        snprintf(keyName, 6, "SSID%x", i);
        size = 32 * sizeof(uint8_t);
        err = nvs_set_blob(nvsHandle, keyName, (void*)wcfg[i].SSID, size);
        if (err != ESP_OK) {
            nvs_close(nvsHandle);
            return err;
        }
        err = nvs_commit(nvsHandle);
        if (err != ESP_OK) {
            nvs_close(nvsHandle);
            return err;
        }

        snprintf(keyName, 6, "PASS%x", i);
        size = 64 * sizeof(uint8_t);
        err = nvs_set_blob(nvsHandle, keyName, (void*)wcfg[i].PASS, size);
        if (err != ESP_OK) {
            nvs_close(nvsHandle);
            return err;
        }
        err = nvs_commit(nvsHandle);
        if (err != ESP_OK) {
            nvs_close(nvsHandle);
            return err;
        }
    }

    size = sizeof(tcpip_adapter_ip_info_t);
    err = nvs_set_blob(nvsHandle, "IPv4", (void*)&ipInfo, size);
    if (err != ESP_OK) {
        nvs_close(nvsHandle);
        return err;
    }
    err = nvs_commit(nvsHandle);
    if (err != ESP_OK) {
        nvs_close(nvsHandle);
        return err;
    }

    nvs_close(nvsHandle);
    return err;
}
