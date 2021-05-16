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
#include "DLEDController.h"
#include "light-effect.h"

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
DStripData stripL, stripR;
DLEDController LEDcontroller;
ESP32RMTChannel rmt0, rmt1;

LightEffect effL, effR;
const uint32_t noAnimation = 0xFFFFFFFF;
uint32_t animationID = 0;
uint32_t httpAnimationID = noAnimation;

uint32_t currentColor = 0xFF00FF;
uint32_t currentIntensity = 2; // 0 ... 100

static SemaphoreHandle_t displayMutex = NULL;
static TaskHandle_t xDisplayTask = NULL;
static TaskHandle_t xHTTPHandlerTask = NULL;
static TaskHandle_t xLoopTask = NULL;

extern "C" {

    void RGBadjust(ColorWRGB& rgb, uint32_t val) {
        uint32_t r, g, b;

        b = val & 0x0000FF;
        g = (val >> 8) & 0x0000FF;
        r = (val >> 16) & 0x0000FF;

        uint32_t ci;

        ci = currentIntensity > 100 ? 100 : currentIntensity;
        rgb.r = r * ci / 100;
        rgb.g = g * ci / 100;
        rgb.b = b * ci / 100;
    }

#define STATIC_RAINBOW

    void updateAnimationID(uint32_t val) {
        animationID = val;
        board.httpServer.animationID = val;

        uint32_t delayR = 20;
        uint8_t stepLen = 9;
#ifdef STATIC_RAINBOW
        uint32_t waitDelay = 5000;
#endif

        bool updateStrip = false;

        switch (animationID) {
        case 0:
            effL.Initialize(Effect::color); effL.useGammaCorrection = false;
            effR.Initialize(Effect::color); effR.useGammaCorrection = false;
            RGBadjust(effL.color0, currentColor);
            RGBadjust(effR.color0, currentColor);
            break;

        case 1:
        case 3:
        case 5:
            effL.Initialize(Effect::blink);
            effL.color0.Set(0x010101); effL.delay0 = 1000; effL.color1.Set((uint32_t)0); effL.delay1 = 1000;
            effL.useGammaCorrection = false; effL.stopWhenCycleCompleted = true;
            effR.Initialize(Effect::blink);
            effR.color0.Set((uint32_t)0); effR.delay0 = 1000; effR.color1.Set(0x010101); effR.delay1 = 1000;
            effR.useGammaCorrection = false; effR.stopWhenCycleCompleted = true;
            break;

        case 2:
            effL.Initialize(Effect::rainbow);
            effL.delay0 = delayR; effL.hsvBase.s = 0xFF; effL.hsvBase.v = 0xFF; effL.stopWhenCycleCompleted = true;
            effL.hueInc = true; effL.hueStep = 2 * stepLen; effL.constantEnergy = false; effL.useGammaCorrection = false;
            effR.Initialize(Effect::rainbow);
            effR.delay0 = delayR; effR.hsvBase.s = 0xFF; effR.hsvBase.v = 0xFF; effR.stopWhenCycleCompleted = true;
            effR.hueInc = true; effR.hueStep = stepLen; effR.constantEnergy = true; effR.useGammaCorrection = false;

#ifdef STATIC_RAINBOW
            effL.Step(0); effR.Step(0); updateStrip = true;
            effL.Initialize(Effect::delay); effL.delay0 = waitDelay;
            effR.Initialize(Effect::delay); effR.delay0 = waitDelay;
#endif
            break;

        case 4:
            effL.Initialize(Effect::rainbow);
            effL.delay0 = delayR; effL.hsvBase.s = 0xFF; effL.hsvBase.v = 0xFF; effL.stopWhenCycleCompleted = true;
            effL.hueInc = true; effL.hueStep = stepLen; effL.constantEnergy = true; effL.useGammaCorrection = false;
            effR.Initialize(Effect::rainbow);
            effR.delay0 = delayR; effR.hsvBase.s = 0xFF; effR.hsvBase.v = 0xFF; effR.stopWhenCycleCompleted = true;
            effR.hueInc = true; effR.hueStep = stepLen; effR.constantEnergy = true; effR.useGammaCorrection = true;

#ifdef STATIC_RAINBOW
            effL.Step(0); effR.Step(0); updateStrip = true;
            effL.Initialize(Effect::delay); effL.delay0 = waitDelay;
            effR.Initialize(Effect::delay); effR.delay0 = waitDelay;
#endif
            break;

        case 6:
            effL.Initialize(Effect::rainbow);
            effL.delay0 = delayR; effL.hsvBase.s = 0xFF; effL.hsvBase.v = 0xFF; effL.stopWhenCycleCompleted = true;
            effL.hueInc = true; effL.hueStep = 2 * stepLen; effL.constantEnergy = false; effL.useGammaCorrection = false;
            effR.Initialize(Effect::rainbow);
            effR.delay0 = delayR; effR.hsvBase.s = 0xFF; effR.hsvBase.v = 0xFF; effR.stopWhenCycleCompleted = true;
            effR.hueInc = true; effR.hueStep = 2 * stepLen; effR.constantEnergy = false; effR.useGammaCorrection = true;

#ifdef STATIC_RAINBOW
            effL.Step(0); effR.Step(0); updateStrip = true;
            effL.Initialize(Effect::delay); effL.delay0 = waitDelay;
            effR.Initialize(Effect::delay); effR.delay0 = waitDelay;
#endif
            break;

        default:
            break;
        }

        if (updateStrip) {
            LEDcontroller.SetLEDs(stripL, rmt0);
            LEDcontroller.SetLEDs(stripR, rmt1);
        }
    }

    void updateCurrentColor(uint32_t val) {
        currentColor = val;
        board.httpServer.currentColor = val;
    }

    void updateCurrentIntensity(uint32_t val) {
        currentIntensity = val;
        board.httpServer.currentIntensity = val;
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
        TickType_t xLastWakeTime;
        const TickType_t xFrequency = 20 / portTICK_PERIOD_MS;
        int64_t usSinceBoot = 0;
        uint32_t msTime = 0;

        stripL.Create(cfgLEDcount);
        stripR.Create(cfgLEDcount);
        rmt0.Initialize((rmt_channel_t)cfgChannelL, (gpio_num_t)cfgOutputPinL, cfgLEDcount * 24);
        rmt0.ConfigureForWS2812x();
        rmt1.Initialize((rmt_channel_t)cfgChannelR, (gpio_num_t)cfgOutputPinR, cfgLEDcount * 24);
        rmt1.ConfigureForWS2812x();

        LEDcontroller.SetMutex(displayMutex);
        LEDcontroller.SetLEDType(LEDType::WS2812);
        effL.SetDataBuffer(stripL.Data(), stripL.DataLength());
        effR.SetDataBuffer(stripR.Data(), stripR.DataLength());

        animationID = 1;
        updateCurrentColor(currentColor);
        updateCurrentIntensity(currentIntensity);
        updateAnimationID(animationID);
        httpAnimationID = noAnimation;
        uint32_t nextAnimationID = noAnimation;
        LEDcontroller.SetLEDs(stripL, rmt0);
        LEDcontroller.SetLEDs(stripR, rmt1);

        xLastWakeTime = xTaskGetTickCount();
        for(;;) {
            usSinceBoot = esp_timer_get_time();
            msTime = (uint32_t)(usSinceBoot / 1000);

            if (httpAnimationID != noAnimation) {
                updateAnimationID(httpAnimationID);
                httpAnimationID = noAnimation;
            }

            if (xSemaphoreTake(displayMutex, portMAX_DELAY) == pdFALSE) {
                vTaskDelay(1);
            }
            else {
                bool updateStrip = false;

                if (animationID < 7) {
                    if (effL.Step(msTime)) updateStrip = true;
                    if (effR.Step(msTime)) updateStrip = true;
                    if (!effL.IsRunning()) {
                        nextAnimationID = animationID + 1;
                    }
                }
                else {
                    nextAnimationID = 0;
                }

                xSemaphoreGive(displayMutex);

                if (updateStrip) {
                    LEDcontroller.SetLEDs(stripL, rmt0);
                    LEDcontroller.SetLEDs(stripR, rmt1);
                }

                if ((nextAnimationID != animationID) && (nextAnimationID != noAnimation)) {
                    updateAnimationID(nextAnimationID);
                    nextAnimationID = noAnimation;
                }

                vTaskDelayUntil(&xLastWakeTime, xFrequency);
            }
        }
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
                        updateCurrentColor(0);
                        httpAnimationID = 0;
                        break;
                    case 2:
                        updateCurrentColor(httpCmd.data);
                        httpAnimationID = 0;
                        break;
                    case 3:
                        updateCurrentIntensity(httpCmd.data);
                        httpAnimationID = 0;
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

        displayMutex = xSemaphoreCreateMutex();
        if (displayMutex != NULL) {
            ESP_LOGI(TAG, "Display mutex created.");
        }
        else {
            ESP_LOGE(TAG, "Failed to create the display mutex !");
            board.PowerPeripherals(false);
            board.EnterDeepSleep(60);
        }

        xTaskCreatePinnedToCore(DisplayTask, "Display task", 2048, NULL, uxTaskPriorityGet(NULL) + 5, &xDisplayTask, (BaseType_t)1);
        if (xDisplayTask != NULL) {
            ESP_LOGI(TAG, "Display task created.");
        }
        else {
            ESP_LOGE(TAG, "Failed to create the display task !");
            board.PowerPeripherals(false);
            board.EnterDeepSleep(60);
        }

        xTaskCreatePinnedToCore(HTTPTask, "HTTP command handling task", 2048, NULL, uxTaskPriorityGet(NULL) + 1, &xHTTPHandlerTask, (BaseType_t)0);
        if (xHTTPHandlerTask != NULL) {
            ESP_LOGI(TAG, "HTTP command handling task created.");
        }
        else {
            ESP_LOGE(TAG, "Failed to create the HTTP command handling task !");
            board.PowerPeripherals(false);
            board.EnterDeepSleep(60);
        }

        xTaskCreatePinnedToCore(LoopTask, "Loop task", 4096, NULL, uxTaskPriorityGet(NULL) + 2, &xLoopTask, (BaseType_t)0);
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
