/**
This file is part of pax-LampD1 (https://github.com/CalinRadoni/pax-LampD1)
Copyright (C) 2019 by Calin Radoni

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
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include <string.h>

#include "driver/ledc.h"

#include "BoardLampD1.h"

#include "esp32_hal_timers.h"
#include "DStrip.h"
#include "DLEDController.h"

#include "sdkconfig.h"

const char* TAG = "main.cpp";

static const uint8_t  cfgOutputPinL = 14;    // the GPIO where LEDs are connected
static const uint8_t  cfgOutputPinR = 13;    // the GPIO where LEDs are connected
static const uint8_t  cfgChannelL   = 0;     // ESP32 RMT's channel [0 ... 7]
static const uint8_t  cfgChannelR   = 1;     // ESP32 RMT's channel [0 ... 7]
static const uint16_t cfgLEDcount   = 80;    // number of LEDS
static const uint8_t  cfgMaxCCV     = 32;    // maximum value allowed for color component

static const uint32_t timer00period = 5; // ms
static const uint32_t timer00ticks100ms  =  100 / timer00period; // ticks for 100 ms
static const uint32_t timer00ticks1000ms = 1000 / timer00period; // ticks for 1 second

// TODO: Make a class to control the onboard LED based on the driver/ledc API

BoardLampD1 board;
bool stationMode;
esp32hal::Timers timers;
DStrip stripL, stripR;
DLEDController LEDcontroller;
ESP32RMTChannel rmt0, rmt1;

uint32_t animationID = 1;
uint32_t currentColor = 0xFF00FF;
uint32_t currentIntensity = 10; // 0 ... 100

static SemaphoreHandle_t displayMutex = NULL;
static TaskHandle_t xDisplayTask = NULL;
static TaskHandle_t xAnimationTask = NULL;
static TaskHandle_t xHTTPHandlerTask = NULL;
static TaskHandle_t xLoopTask = NULL;

extern "C" {

    uint32_t RGBadjusted (uint32_t val) {
        uint32_t r, g, b;

        b = val & 0x0000FF;
        g = (val >> 8) & 0x0000FF;
        r = (val >> 16) & 0x0000FF;

        uint32_t ci;

        ci = currentIntensity > 100 ? 100 : currentIntensity;
        r = r * ci / 100;
        g = g * ci / 100;
        b = b * ci / 100;

        return (r << 16) | (g << 8) | b;
    }

    static void LoopTask(void *taskParameter) {
        uint32_t secondTick = 0;
        uint32_t timerTicks = 0;
        uint16_t keyPressCount = 0;

        uint32_t tens = 0;

        board.debouncer.SetUpdateTime(timer00period);
        board.debouncer.SetKeyRepeat(500, 50);

        const TickType_t xBlockTime = 1000 / portTICK_PERIOD_MS; // set timeout to 1000ms
        for(;;) {
            uint32_t notifiedValue = ulTaskNotifyTake(pdTRUE, xBlockTime);
            if (notifiedValue == 0) {
                // timeout
            }
            else {
                // read and debounce onboard button
                board.debouncer.Update(board.OnboardButtonPressed());

                timerTicks += notifiedValue;
                if (timerTicks >= timer00ticks100ms) {
                    timerTicks -= timer00ticks100ms; // at least 100 ms passed

                    if (board.debouncer.IsDown()) {
                        // on-board button is pressed
                        keyPressCount = board.debouncer.GetCurrentPressCount();
                        if (keyPressCount != 0) {
                            // do something with it, if you want
                        }
                    }
                    else {
                        // on-board button is not pressed
                        if (keyPressCount != 0) {
                            // on-board button was just released, do something with keyPressCount, if you want

                            keyPressCount = 0; // set this to 0 for next iterations
                        }
                    }
                }

                secondTick += notifiedValue;
                if (secondTick >= timer00ticks1000ms) {
                    secondTick -= timer00ticks1000ms; // at least 1 second passed

                    if (stationMode) {
                        if (!board.IsConnectedToAP()) {
                            // the board has lost the WiFi connectivity
                            board.StopTheServers();
                            if (board.RestartStationMode(3) == ESP_OK) {
                                board.StartTheServers();
                                board.ConfigureMDNS();
                            }
                            else {
                                // failed to connect to WiFi
                            }
                        }
                    }

                    tens++;
                    if (tens >= 60) {
                        tens -= 60;
                        board.RefreshSystemState(true);
                    }
                }
            }
        }

        // the next lines are here only for "completion"
        timers.DisableTimer(timer_group_t::TIMER_GROUP_0, timer_idx_t::TIMER_0);
        vTaskDelete(NULL);
    }

    static void DisplayTask(void *taskParameter) {
        stripL.Create(3, cfgLEDcount, cfgMaxCCV);
        stripR.Create(3, cfgLEDcount, cfgMaxCCV);
        rmt0.Initialize((rmt_channel_t)cfgChannelL, (gpio_num_t)cfgOutputPinL, cfgLEDcount * 24);
        rmt0.ConfigureForWS2812x();
        rmt1.Initialize((rmt_channel_t)cfgChannelR, (gpio_num_t)cfgOutputPinR, cfgLEDcount * 24);
        rmt1.ConfigureForWS2812x();

        LEDcontroller.SetMutex(displayMutex);
        LEDcontroller.SetLEDType(LEDType::WS2812);
        LEDcontroller.SetLEDs(stripL.description.data, stripL.description.dataLen, &rmt0);
        LEDcontroller.SetLEDs(stripR.description.data, stripR.description.dataLen, &rmt1);

        const TickType_t xBlockTime = 1000 / portTICK_PERIOD_MS; // set timeout to 1000ms
        for(;;) {
            uint32_t res = ulTaskNotifyTake(pdTRUE, xBlockTime);
            if (res == 0) {
                // timeout
            }
            else {
                LEDcontroller.SetLEDs(stripL.description.data, stripL.description.dataLen, &rmt0);
                LEDcontroller.SetLEDs(stripR.description.data, stripR.description.dataLen, &rmt1);
            }
        }
        vTaskDelete(NULL);
    }

    static void AnimationTask(void *taskParameter) {
        uint16_t step = 0;
        uint32_t nextInterval;
        const TickType_t xBlockTime = 1000 / portTICK_PERIOD_MS; // set timeout to 1000ms
        for(;;) {
            uint32_t res = ulTaskNotifyTake(pdTRUE, xBlockTime);
            if (res == 0) {
                // timeout
            }
            else {
                // set next timer alarm period
                switch (animationID) {
                    case 0 : nextInterval = 250; break;
                    case 1 : nextInterval = 20; break;
                    case 2 : nextInterval = 50; break;
                    default: nextInterval = 250; break;
                }
                timers.RestartTimer(0, 1, nextInterval);

                if (displayMutex != NULL) {
                    if (xSemaphoreTake(displayMutex, xBlockTime) == pdTRUE) {
                        // compute current frame
                        switch (animationID) {
                            case 0:
                                {
                                    uint32_t adj = RGBadjusted(currentColor);
                                    for (uint16_t i = 0; i < cfgLEDcount; ++i) {
                                        stripL.SetPixel(i, adj);
                                        stripR.SetPixel(i, adj);
                                    }
                                }
                                break;

                            case 1:
                                stripL.MovePixel(step);
                                stripR.MovePixel(step);
                                step++;
                                if (step >= (6 * stripL.description.stripLen)) {
                                    step = 0;
                                    animationID = 2;
                                }
                                break;

                            case 2:
                                stripL.RainbowStep(step);
                                stripR.RainbowStep(1024 - step);
                                step++;
                                if (step >= 1024) {
                                    step = 0;
                                    currentColor = 0xFF00FF;
                                    currentIntensity = 2;
                                    animationID = 0;
                                }
                                break;

                            default: break;
                        }

                        xSemaphoreGive(displayMutex);
                    }
                }

                // notify the display task
                if (xDisplayTask != NULL) {
                    xTaskNotifyGive(xDisplayTask);
                }
            }
        }
        timers.DisableTimer(0, 1);
        vTaskDelete(NULL);
    }

    static void HTTPTask(void *taskParameter) {
        HTTPCommand httpCmd;

        QueueHandle_t serverQueue;
        serverQueue = board.GetHttpServerQueue();

        for(;;) {
            if (xQueueReceive(serverQueue, &httpCmd, portMAX_DELAY) == pdPASS) {
                switch (httpCmd.command) {
                    case 0: // nop-like command
                        break;
                    case 1:
                        currentColor = 0;
                        animationID = 0;
                        break;
                    case 2:
                        currentColor = httpCmd.data;
                        animationID = 0;
                        break;
                    case 3:
                        currentIntensity = httpCmd.data;
                        animationID = 0;
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
        board.CheckApplicationImage();

        esp_err_t err = board.Initialize();
        if (err != ESP_OK) {
            uint8_t initFailSeverity = board.InitFailSeverity();

            board.PowerPeripherals(false);

            ESP_LOGE(TAG, "Initialization failed with severity level %d !", initFailSeverity);
            if (initFailSeverity == 5) {
                // critical system error
                board.EnterDeepSleep(60);
            }

            // retry in a few minutes, maybe it will recover ?
            board.Restart(120 + (esp_random() & 0x3F));
        }
        ESP_LOGI(TAG, "Board initialized OK");

        stationMode = board.IsConnectedToAP();

        board.httpServer.animationID = animationID;
        board.httpServer.currentColor = currentColor;
        board.httpServer.currentIntensity = currentIntensity;

        displayMutex = xSemaphoreCreateMutex();
        if (displayMutex != NULL) {
            ESP_LOGI(TAG, "Display mutex created.");
        }
        else {
            ESP_LOGE(TAG, "Failed to create the display mutex !");
            board.PowerPeripherals(false);
            board.EnterDeepSleep(60);
        }

        xTaskCreate(DisplayTask, "Display task", 2048, NULL, uxTaskPriorityGet(NULL) + 3, &xDisplayTask);
        if (xDisplayTask != NULL) {
            ESP_LOGI(TAG, "Display task created.");
        }
        else {
            ESP_LOGE(TAG, "Failed to create the display task !");
            board.PowerPeripherals(false);
            board.EnterDeepSleep(60);
        }

        xTaskCreate(AnimationTask, "Animation task", 2048, NULL, uxTaskPriorityGet(NULL) + 5, &xAnimationTask);
        if (xAnimationTask != NULL) {
            ESP_LOGI(TAG, "Animation task created.");
        }
        else {
            ESP_LOGE(TAG, "Failed to create the animation task !");
            board.PowerPeripherals(false);
            board.EnterDeepSleep(60);
        }
        if (!timers.EnableTimer(xAnimationTask, 0, 1, 100, false, true)) {
            ESP_LOGE(TAG, "Failed to enable the timer 0:1 !");
            board.PowerPeripherals(false);
            board.EnterDeepSleep(60);
        }

        xTaskCreate(HTTPTask, "HTTP command handling task", 2048, NULL, uxTaskPriorityGet(NULL) + 1, &xHTTPHandlerTask);
        if (xHTTPHandlerTask != NULL) {
            ESP_LOGI(TAG, "HTTP command handling task created.");
        }
        else {
            ESP_LOGE(TAG, "Failed to create the HTTP command handling task !");
            board.PowerPeripherals(false);
            board.EnterDeepSleep(60);
        }

        xTaskCreate(LoopTask, "Loop task", 4096, NULL, uxTaskPriorityGet(NULL) + 2, &xLoopTask);
        if (xLoopTask != NULL) {
            ESP_LOGI(TAG, "Loop task created.");
        }
        else {
            ESP_LOGE(TAG, "Failed to create the Loop task !");
            board.PowerPeripherals(false);
            board.EnterDeepSleep(60);
        }
        if (!timers.EnableTimer(xLoopTask, 0, 0, timer00period, true, false)) {
            ESP_LOGE(TAG, "Failed to enable the timer 0:0 !");
            board.PowerPeripherals(false);
            board.EnterDeepSleep(60);
        }
    }
}
