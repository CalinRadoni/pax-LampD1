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

#ifndef BoardLampD1_H
#define BoardLampD1_H

#include "Board.h"
#include "Debouncer.h"
#include "HTTPSrvLampD1.h"

/**
 * Hardware
 *
 * Onboard LED IO2 to ground
 * Boot button IO0 to ground (needs pull-up)
 *
 * SCL IO22 with 2k2 pull-up
 * SDA IO21 with 2k2 pull-up
 *
 * LED output 1 IO14
 * LED output 2 IO13
 * extPwr IO26, 1 = power on, 0 = power off (controls a NMOS-PMOS pair)
 */

class BoardLampD1 : public Board
{
public:
    BoardLampD1(void);
    virtual ~BoardLampD1(void);

    /**
     * @brief Perform basic hardware initialization
     *
     * This function initialize the pins for hardware connections
     */
    virtual esp_err_t EarlyInit(void);

    virtual esp_err_t CriticalInit(void);

    virtual esp_err_t BoardInit(void);

    virtual esp_err_t PostInit(void);

    virtual bool PowerPeripherals(bool);

    esp_err_t StartTheServers(void);
    void StopTheServers(void);
    esp_err_t ConfigureMDNS(void);

    /** To be used with the onboard button */
    Debouncer debouncer;

    /**
     * @brief Retuns true if the onboard button is pressed
     */
    bool OnboardButtonPressed(void);

    /**
     * @brief The server queue is used to read the commands received by the HTTP server
     */
    QueueHandle_t GetHttpServerQueue(void);

    /**
     * @brief Refresh system state
     *
     * @param printResult Prints system state after refresh
     */
    void RefreshSystemState(bool printResult);

    /**
     * The HTTP server is public to ease the transfer of data from main app to it
     */
    HTTPSrvLampD1 httpServer;

    /**
     * Objects for hardware interfaces
     */
    esp32hal::GPIO gpio;

protected:
private:
};

#endif
