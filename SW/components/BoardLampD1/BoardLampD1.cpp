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

#include "driver/gpio.h"

#include "BoardLampD1.h"

// -----------------------------------------------------------------------------

static const char* TAG = "BoardLampD1";

// -----------------------------------------------------------------------------

const gpio_num_t GPIO_BLED  = (gpio_num_t)2;
const gpio_num_t GPIO_BOOT  = (gpio_num_t)0;

const gpio_num_t GPIO_Power = (gpio_num_t)26;
const gpio_num_t GPIO_Out1  = (gpio_num_t)14;
const gpio_num_t GPIO_Out2  = (gpio_num_t)13;

// -----------------------------------------------------------------------------

BoardLampD1::BoardLampD1(void) : Board()
{
    configuration = new Configuration();
}

BoardLampD1::~BoardLampD1(void)
{
    if (configuration != nullptr) {
        delete configuration;
        configuration = nullptr;
    }
}

esp_err_t BoardLampD1::EarlyInit(void)
{
    esp32hal::GPIO gpio;
    esp32hal::ADC adc;
    esp32hal::I2C i2c0;
    esp32hal::SPI hspi;

    // onboard "button"
    gpio.ModeInput(GPIO_BOOT, GPIO_PULLUP_ENABLE, GPIO_PULLDOWN_DISABLE);

    // LED output 1 -> 0V
    gpio.ModeOutput(GPIO_Out1, 0);
    // LED output 2 -> 0V
    gpio.ModeOutput(GPIO_Out2, 0);
    // Output power OFF
    gpio.ModeOutput(GPIO_Power, 0);
    // onboard LED to off
    gpio.ModeOutput(GPIO_BLED, 0);

    return ESP_OK;
}

esp_err_t BoardLampD1::CriticalInit(void)
{
    return ESP_OK;
}

esp_err_t BoardLampD1::BoardInit(void)
{
    esp_err_t res = InitializeWiFi();
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "0x%x Failed to initialize WiFi !", res);
        return res;
    }

    if (!httpServer.Initialize())
        return ESP_FAIL;

    return ESP_OK;
}

esp_err_t BoardLampD1::PostInit(void)
{
    esp_err_t res = StartStation();
    if (res != ESP_OK) {
        // failed to connect to an AP
        ESP_LOGW(TAG, "0x%x Failed to connect to an AP", res);

        res = StartAP();
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "0x%x Failed to start AP mode !", res);
            return res;
        }

        ESP_LOGI(TAG, "Started in AP mode");
    }

    res = httpServer.StartServer(&simpleOTA, configuration);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "0x%x Failed to start HTTP server !", res);
        return res;
    }

    return ESP_OK;
}

void BoardLampD1::StopCurrentWiFiMode(void)
{
    httpServer.StopServer();
    StopWiFiMode();
}

void BoardLampD1::PowerOn(void)
{
    gpio_set_level(GPIO_Power, 1);
}

void BoardLampD1::PowerOff(void)
{
    gpio_set_level(GPIO_Power, 0);
}

bool BoardLampD1::OnboardButtonPressed(void)
{
    return (gpio_get_level(GPIO_BOOT) == 0) ? true : false;
}

QueueHandle_t BoardLampD1::GetHttpServerQueue(void)
{
    return httpServer.GetQueueHandle();
}
