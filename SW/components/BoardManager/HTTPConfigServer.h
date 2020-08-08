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

#ifndef HTTPConfigServer_H
#define HTTPConfigServer_H

#include "freertos/FreeRTOS.h"
#include "esp_http_server.h"

#include "BoardEvents.h"

enum class CredentialsState
{
    notSet,
    checking,
    invalidSSID,
    invalidPass,
    processingError,
    checkFail,
    checkSuccess,
    checkTimeout
};

enum class CredentialsSaveState
{
    notSet,
    saved,
    saveRestart,
    saveError
};

class HTTPConfigServer
{
public:
    HTTPConfigServer(void);
    virtual ~HTTPConfigServer(void);

    EventGroupHandler *events;

    esp_err_t StartServer(void);
    void StopServer(void);

    esp_err_t HandleRequest(httpd_req_t*);

    char SSID[32];
    char PASS[64];

    bool credentialsReceived;
    bool checkThem;
    bool saveThem;

    CredentialsState credentialsState;
    CredentialsSaveState credentialsSaveState;

private:
    httpd_handle_t serverHandle;

    esp_err_t HandleGetRequest(httpd_req_t*);
    esp_err_t HandlePostRequest(httpd_req_t*);

    bool working;

    void SendMainPage(httpd_req_t* req);
};

extern HTTPConfigServer theHTTPConfigServer;

#endif
