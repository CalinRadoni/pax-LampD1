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

#include "BoardDev34.h"
#include "HTTPC2Server.h"

// -----------------------------------------------------------------------------

const gpio_num_t GPIO_BLED  = (gpio_num_t)2;
const gpio_num_t GPIO_BOOT  = (gpio_num_t)0;

const gpio_num_t GPIO_Power = (gpio_num_t)26;
const gpio_num_t GPIO_Out1  = (gpio_num_t)14;
const gpio_num_t GPIO_Out2  = (gpio_num_t)13;

// -----------------------------------------------------------------------------

BoardDev34::BoardDev34(void) : Board()
{
    //
}

BoardDev34::~BoardDev34(void)
{
    //
}

esp_err_t BoardDev34::EarlyInit(void)
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

/*
    gpio_config_t gc;

    // onboard "button"
    gc.pin_bit_mask = 1 << GPIO_BOOT;
    gc.mode         = GPIO_MODE_INPUT;
    gc.pull_up_en   = GPIO_PULLUP_ENABLE;
    gc.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gc.intr_type    = GPIO_INTR_DISABLE;
    gpio_config(&gc);
    gpio_pad_select_gpio(GPIO_BOOT);

    // LED output 1 -> 0V
    // LED output 2 -> 0V
    // Output power OFF
    // onboard LED to off
    gc.pin_bit_mask = (1ULL << GPIO_Out1) | (1ULL << GPIO_Out2) | (1ULL << GPIO_Power) | (1ULL << GPIO_BLED);
    gc.mode         = GPIO_MODE_OUTPUT;
    gc.pull_up_en   = GPIO_PULLUP_DISABLE;
    gc.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gc.intr_type    = GPIO_INTR_DISABLE;
    gpio_config(&gc);

    gpio_set_level(GPIO_Out1,  0);
    gpio_set_level(GPIO_Out2,  0);
    gpio_set_level(GPIO_Power, 0);
    gpio_set_level(GPIO_BLED,  0);

    gpio_pad_select_gpio(GPIO_Out1);
    gpio_pad_select_gpio(GPIO_Out2);
    gpio_pad_select_gpio(GPIO_Power);
    gpio_pad_select_gpio(GPIO_BLED);
*/
    return ESP_OK;
}

esp_err_t BoardDev34::CriticalInit(void)
{
    return ESP_OK;
}

esp_err_t BoardDev34::BoardInit(void)
{
    return ESP_OK;
}

void BoardDev34::PowerOn(void)
{
    gpio_set_level(GPIO_Power, 1);
}

void BoardDev34::PowerOff(void)
{
    gpio_set_level(GPIO_Power, 0);
}

bool BoardDev34::OnboardButtonPressed(void)
{
    return (gpio_get_level(GPIO_BOOT) == 0) ? true : false;
}
