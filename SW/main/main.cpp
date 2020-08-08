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
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include <string.h>

#include "driver/ledc.h"

#include "BoardDev34.h"

#include "ESP32SimpleOTA.h"
#include "DStrip.h"
#include "DLEDController.h"
#include "ESP32Timers.h"
#include "HTTPC2Server.h"

#include "sdkconfig.h"

const char* TAG = "Init";

static const uint8_t  cfgOutputPinL = 14;    // the GPIO where LEDs are connected
static const uint8_t  cfgOutputPinR = 13;    // the GPIO where LEDs are connected
static const uint8_t  cfgChannelL   = 0;     // ESP32 RMT's channel [0 ... 7]
static const uint8_t  cfgChannelR   = 1;     // ESP32 RMT's channel [0 ... 7]
static const uint16_t cfgLEDcount   = 80;    // 64 LEDS
static const uint8_t  cfgMaxCCV     = 32;    // maximum value allowed for color component

static const uint32_t dTimerPeriod = 5; // ms
static const uint32_t dTimerPPS = 1000 / dTimerPeriod;

// TODO: Make a class to control the onboard LED based on the driver/ledc API

BoardDev34 board;
DStrip stripL, stripR;
DLEDController LEDcontroller;
ESP32RMTChannel rmt0, rmt1;
HTTPC2Server theHTTPServer;

extern "C" {

    void delay_ms(uint32_t ms)
    {
        if (ms != 0) {
            vTaskDelay(ms / portTICK_PERIOD_MS);
        }
    }

    static void TimerTask(void *taskParameter) {
        uint32_t animationTick = 0;
        uint32_t secondTick = 0;
        ESP32TimerEvent timerEvent;
        uint16_t step = 0;
        uint16_t keyPressCount = 0;

        stripL.Create(3, cfgLEDcount, cfgMaxCCV);
        stripR.Create(3, cfgLEDcount, cfgMaxCCV);
        rmt0.Initialize((rmt_channel_t)cfgChannelL, (gpio_num_t)cfgOutputPinL, cfgLEDcount * 24);
        rmt0.ConfigureForWS2812x();
        rmt1.Initialize((rmt_channel_t)cfgChannelR, (gpio_num_t)cfgOutputPinR, cfgLEDcount * 24);
        rmt1.ConfigureForWS2812x();

        LEDcontroller.SetLEDType(LEDType::WS2812);
        LEDcontroller.SetLEDs(stripL.description.data, stripL.description.dataLen, &rmt0);
        LEDcontroller.SetLEDs(stripR.description.data, stripR.description.dataLen, &rmt1);

        board.PowerOn();

        board.debouncer.SetUpdateTime(dTimerPeriod);
        board.debouncer.SetKeyRepeat(500, 50);

        while (step < 6 * stripL.description.stripLen) {
            stripL.MovePixel(step);
            stripR.MovePixel(step);
            step++;
            LEDcontroller.SetLEDs(stripL.description.data, stripL.description.dataLen, &rmt0);
            LEDcontroller.SetLEDs(stripR.description.data, stripR.description.dataLen, &rmt1);
            delay_ms(20);
        }

        step = 0;
        for(;;) {
            if (xQueueReceive(timers.timerQueue, &timerEvent, portMAX_DELAY) == pdPASS) {
                if (timerEvent.group == timer_group_t::TIMER_GROUP_0) {
                    if (timerEvent.index == timer_idx_t::TIMER_0) {
                        // read and debounce onboard button
                        board.debouncer.Update(board.OnboardButtonPressed());

                        animationTick++;
                        if (animationTick >= 20) {
                            animationTick -= 20;
                            if (board.debouncer.IsDown()) {
                                // on-board button is pressed
                                keyPressCount = board.debouncer.GetCurrentPressCount();
                                stripL.SetColorByIndex(0, step + keyPressCount);
                                stripR.SetColorByIndex(0, step + keyPressCount);
                            }
                            else {
                                // on-board button is not pressed
                                if (keyPressCount != 0) {
                                    // on-board button was just released
                                    step += keyPressCount;
                                    keyPressCount = 0;
                                }
                            }

                            // Display a frame
                            LEDcontroller.SetLEDs(stripL.description.data, stripL.description.dataLen, &rmt0);
                            LEDcontroller.SetLEDs(stripR.description.data, stripR.description.dataLen, &rmt1);
                        }

                        secondTick++;
                        if (secondTick >= dTimerPPS) {
                            secondTick -= dTimerPPS;
                        }
                    }
                }
            }
        }

        // the next lines are here only for "completion"
        timers.DestroyTimer(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0);
        timers.Destroy();
        vTaskDelete(NULL);
    }

    static void HTTPTask(void *taskParameter) {
        HTTPCommand httpCmd;

        for(;;) {
            if (xQueueReceive(theHTTPServer.serverQueue, &httpCmd, portMAX_DELAY) == pdPASS) {
                switch (httpCmd.command) {
                    case 0:
                        for (uint16_t i = 0; i < cfgLEDcount; i++) {
                            stripL.SetPixel(i, 0);
                            stripR.SetPixel(i, 0);
                        }
                        LEDcontroller.SetLEDs(stripL.description.data, stripL.description.dataLen, &rmt0);
                        LEDcontroller.SetLEDs(stripR.description.data, stripR.description.dataLen, &rmt1);
                        break;
                    case 1:
                        for (uint16_t i = 0; i < cfgLEDcount; i++) {
                            stripL.SetPixel(i, httpCmd.data);
                            stripR.SetPixel(i, httpCmd.data);
                        }
                        LEDcontroller.SetLEDs(stripL.description.data, stripL.description.dataLen, &rmt0);
                        LEDcontroller.SetLEDs(stripR.description.data, stripR.description.dataLen, &rmt1);
                        break;

                    case 0xFE:
                        vTaskDelay (2000 / portTICK_PERIOD_MS);
                        esp_restart();
                        break;

                    default: break;
                }
            }
        }

        // the next lines are here only for "completion"
        vTaskDelete(NULL);
    }

    void app_main()
    {
        simpleOTA.CheckApplicationImage();

        esp_err_t err = board.Initialize();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Initialization failed !");
            board.DoNothingForever();
        }

        err = board.StartAP();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to start AP mode !");
            board.DoNothingForever();
        }

        err = theHTTPServer.StartServer();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to start HTTP server !");
            board.DoNothingForever();
        }

        if (!timers.Create()) {
            ESP_LOGE(TAG, "Failed to create the timers object !");
            board.DoNothingForever();
        }

        if (!timers.CreateTimer(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0, dTimerPeriod, true, false)) {
            ESP_LOGE(TAG, "Failed to create the timer G0T0 !");
            board.DoNothingForever();
        }

        TaskHandle_t xHandleTimerTask = NULL;
        xTaskCreate(TimerTask, "Timer handling task", 2048, NULL, uxTaskPriorityGet(NULL) + 5, &xHandleTimerTask);
        if (xHandleTimerTask != NULL) {
            ESP_LOGI(TAG, "Timer task created.");
        }
        else {
            ESP_LOGE(TAG, "Failed to create the timer task !");
            board.DoNothingForever();
        }

        TaskHandle_t xHandleHTTP = NULL;
        xTaskCreate(HTTPTask, "HTTP command handling task", 2048, NULL, uxTaskPriorityGet(NULL) + 1, &xHandleHTTP);
        if (xHandleHTTP != NULL) {
            ESP_LOGI(TAG, "HTTP command handling task created.");
        }
        else {
            ESP_LOGE(TAG, "Failed to create the HTTP command handling task !");
            board.DoNothingForever();
        }
    }
}
