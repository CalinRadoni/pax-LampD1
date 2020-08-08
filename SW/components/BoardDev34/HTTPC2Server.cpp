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

#include <string.h>

#include "esp_system.h"

#include "HTTPC2Server.h"

#include "ESP32SimpleOTA.h"

// -----------------------------------------------------------------------------

static const char* TAG = "HTTPC2Srv";

const uint8_t queueLength = 8;

extern const uint8_t index_html_start[] asm("_binary_indexc_html_start");
extern const uint8_t index_html_end[]   asm("_binary_indexc_html_end");
extern const uint8_t jquery_js_start[] asm("_binary_jquery_js_start");
extern const uint8_t jquery_js_end[]   asm("_binary_jquery_js_end");

// -----------------------------------------------------------------------------

static esp_err_t request_handler(httpd_req_t *req)
{
    if (req == nullptr) return ESP_FAIL;

    HTTPC2Server* server = (HTTPC2Server *) req->user_ctx;
    if (server == nullptr) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "UserCTX");
        return ESP_FAIL;
    }

    return server->HandleRequest(req);
}

// -----------------------------------------------------------------------------

HTTPC2Server::HTTPC2Server(void)
{
    serverQueue = 0;
    serverHandle = nullptr;

    working = false;
}

HTTPC2Server::~HTTPC2Server(void)
{
    StopServer();
}

esp_err_t HTTPC2Server::StartServer(void)
{
    if (serverHandle != nullptr)
        StopServer();

    serverQueue = xQueueCreate(queueLength, sizeof(HTTPCommand));
    if (serverQueue == 0) {
        ESP_LOGE(TAG, "xQueueCreate");
        return ESP_FAIL;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.uri_match_fn = httpd_uri_match_wildcard;

    esp_err_t err = httpd_start(&serverHandle, &config);
    if (err != ESP_OK) {
        serverHandle = nullptr;
        ESP_LOGE(TAG, "%d httpd_start", err);
        return err;
    }

    httpd_uri_t uri_get = {
        .uri      = "/*",
        .method   = HTTP_GET,
        .handler  = request_handler,
        .user_ctx = nullptr
    };

    httpd_uri_t uri_post = {
        .uri      = "/*",
        .method   = HTTP_POST,
        .handler  = request_handler,
        .user_ctx = nullptr
    };

    uri_get.user_ctx = this;
    httpd_register_uri_handler(serverHandle, &uri_get);
    uri_post.user_ctx = this;
    httpd_register_uri_handler(serverHandle, &uri_post);

    working = true;

    return err;
}

void HTTPC2Server::StopServer(void)
{
    if (serverHandle == nullptr) return;

    working = false;

    httpd_stop(serverHandle);
    serverHandle = nullptr;

    if (serverQueue != 0) {
        vQueueDelete(serverQueue);
        serverQueue = 0;
    }
}

esp_err_t HTTPC2Server::HandleRequest(httpd_req_t* req)
{
    if (req == nullptr) return ESP_FAIL;

    if (!working) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not working");
        return ESP_FAIL;
    }

    switch (req->method) {
        case HTTP_GET:
            return HandleGetRequest(req);
        case HTTP_POST:
            return HandlePostRequest(req);
        default:
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not implemented");
            return ESP_FAIL;
    }
}

esp_err_t HTTPC2Server::HandleGetRequest(httpd_req_t* req)
{
    if (req == nullptr) return ESP_FAIL;

    if (strcmp(req->uri, "/status.json") == 0) {
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
        httpd_resp_set_hdr(req, "Pragma", "no-cache");

        const uint8_t bufferSize = 16;
        char buffer[bufferSize];
        uint8_t statusVal = 0;
        snprintf(buffer, bufferSize, "{\"status\":%d}\n", statusVal);

        httpd_resp_send(req, buffer, strnlen(buffer, bufferSize));
    }
    else {
        if (strcmp(req->uri, "/jquery.js") == 0) {
            httpd_resp_send(req, (const char *)jquery_js_start, jquery_js_end - jquery_js_start);
        }
        else {
            httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
        }
    }
    return ESP_OK;
}

esp_err_t HTTPC2Server::HandlePostRequest(httpd_req_t* req)
{
    if (req == nullptr) return ESP_FAIL;

    esp_err_t err;

    if (strcmp(req->uri, "/upfw") == 0) {
        err = HandleOTA(req);
        if (err == ESP_OK) {
            httpd_resp_sendstr(req, "OTA OK.");
        }
        else {
            httpd_resp_sendstr(req, "OTA Failed !");
        }
        return err;
    }

    if (strcmp(req->uri, "/do.json") != 0) {
        // page not found
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "404 :)");
        return ESP_FAIL;
    }

    size_t webCmdLen  = httpd_req_get_hdr_value_len(req, "X-User-Cmd");
    size_t webDataLen = httpd_req_get_hdr_value_len(req, "X-User-Data");

    const size_t maxHeaderLen = 10;
    char headerStr[maxHeaderLen + 1];

    if (webCmdLen == 0 || webCmdLen > 9) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "X-User-Cmd");
        return ESP_FAIL;
    }
    if (webDataLen == 0 || webDataLen > 9) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "X-User-Data");
        return ESP_FAIL;
    }

    HTTPCommand cmd;

    err = httpd_req_get_hdr_value_str(req, "X-User-Cmd", headerStr, maxHeaderLen);
    if (err != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "X-User-Cmd");
        return ESP_FAIL;
    }
    headerStr[maxHeaderLen] = 0;
    cmd.command = (uint8_t) strtoul(headerStr, nullptr, 10);

    err = httpd_req_get_hdr_value_str(req, "X-User-Data", headerStr, maxHeaderLen);
    if (err != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "X-User-Data");
        return ESP_FAIL;
    }
    headerStr[maxHeaderLen] = 0;
    cmd.data = (uint32_t) strtoul(headerStr, nullptr, 16);

    ESP_LOGI(TAG, "Command: %d, data: %08x", cmd.command, cmd.data);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
    httpd_resp_set_hdr(req, "Pragma", "no-cache");
    httpd_resp_sendstr(req, "{\"status\":0}");

    if (serverQueue != 0) {
        xQueueSendToBack(serverQueue, &cmd, (TickType_t)0);
    }

    return ESP_OK;
}

esp_err_t HTTPC2Server::HandleOTA(httpd_req_t* req)
{
    const size_t buffSize = 1024;
    char buff[buffSize];
    int pageLen = req->content_len;
    int rxLen;
    bool headerReceived = false;
    bool done = false;
    esp_err_t result = ESP_OK;

    if (pageLen > simpleOTA.GetMaxImageSize()) {
        ESP_LOGE(TAG, "OTA content is too big (%d bytes) !", pageLen);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "OTA content is too big !");
        return ESP_FAIL;
    }

    while (!done) {
        size_t reqLen = pageLen > buffSize ? buffSize : pageLen;
        rxLen = httpd_req_recv(req, buff, reqLen);
        if (rxLen < 0) {
            if (rxLen != HTTPD_SOCK_ERR_TIMEOUT) {
                ESP_LOGE(TAG, "httpd_req_recv: %d", rxLen);
                return ESP_FAIL;
            }
        }
        else if (rxLen == 0) {
            done = true;
        }
        else { /* rxLen > 0 */
            if (!headerReceived) {
                headerReceived = true;

                result = simpleOTA.Begin();
                if (result != ESP_OK) { return result; }
            }

            result = simpleOTA.Write(buff, rxLen);
            if (result != ESP_OK) { return result; }

            if (pageLen > rxLen) {
                pageLen -= rxLen;
            }
            else done = true;
        }
    }

    if (!headerReceived) {
        ESP_LOGE(TAG, "Firmware file - no data received");
        return ESP_FAIL;
    }

    result = simpleOTA.End();
    if (result != ESP_OK) { return result; }

    ESP_LOGI(TAG, "OTA done, you should restart");

    return result;
}
