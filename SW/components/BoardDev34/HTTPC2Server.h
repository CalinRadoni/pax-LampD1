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

#ifndef HTTPC2Server_H
#define HTTPC2Server_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_http_server.h"

struct HTTPCommand
{
    uint8_t command;
    uint32_t data;
};

class HTTPC2Server
{
public:
    HTTPC2Server(void);
    virtual ~HTTPC2Server(void);

    /**
     * @brief The queue for timer events
     *
     * The queue is created by the StartServer function and destroyed by the StopServer function or on the destructor
     */
    QueueHandle_t serverQueue;

    esp_err_t StartServer(void);
    void StopServer(void);

    esp_err_t HandleRequest(httpd_req_t*);

private:
    httpd_handle_t serverHandle;

    esp_err_t HandleGetRequest(httpd_req_t*);
    esp_err_t HandlePostRequest(httpd_req_t*);
    esp_err_t HandleOTA(httpd_req_t*);

    bool working;
};

#endif
