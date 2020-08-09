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

#include "HTTPConfigServer.h"

// -----------------------------------------------------------------------------

HTTPConfigServer theHTTPConfigServer;

static const char* TAG = "HTTPCfgSrv";

extern const uint8_t index_html_start[] asm("_binary_indexo_html_start");
extern const uint8_t index_html_end[]   asm("_binary_indexo_html_end");
extern const uint8_t jquery_js_start[] asm("_binary_jquery_js_start");
extern const uint8_t jquery_js_end[]   asm("_binary_jquery_js_end");

// -----------------------------------------------------------------------------

static esp_err_t request_handler(httpd_req_t *req)
{
    if (req == nullptr)
        return ESP_FAIL;

    HTTPConfigServer* server = (HTTPConfigServer *) req->user_ctx;
    if (server == nullptr)
        return ESP_FAIL;

    return server->HandleRequest(req);
}

// -----------------------------------------------------------------------------

HTTPConfigServer::HTTPConfigServer(void)
{
    events = NULL;
    serverHandle = nullptr;

    SSID[0] = 0;
    PASS[0] = 0;

    credentialsReceived = false;
    checkThem = false;
    saveThem = false;

    credentialsState = CredentialsState::notSet;
    credentialsSaveState = CredentialsSaveState::notSet;

    working = false;
}

HTTPConfigServer::~HTTPConfigServer(void)
{
    StopServer();
}

esp_err_t HTTPConfigServer::StartServer(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    credentialsReceived = false;
    checkThem = false;
    saveThem = false;

    if (serverHandle != nullptr)
        StopServer();

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

void HTTPConfigServer::StopServer(void)
{
    if (serverHandle == nullptr) return;

    working = false;

    httpd_stop(serverHandle);
    serverHandle = nullptr;
}

esp_err_t HTTPConfigServer::HandleRequest(httpd_req_t* req)
{
    esp_err_t res = ESP_FAIL;

    if (!working)
        return res;

    if (req == nullptr)
        return res;

    switch (req->method) {
        case HTTP_GET:
            res = HandleGetRequest(req);
            break;
        case HTTP_POST:
            res = HandlePostRequest(req);
            break;
        default:
            break;
    }

    return res;
}

void HTTPConfigServer::SendMainPage(httpd_req_t* req)
{
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
}

esp_err_t HTTPConfigServer::HandleGetRequest(httpd_req_t* req)
{
    if (req == nullptr) return ESP_FAIL;

    ESP_LOGI(TAG, "GET: %s", req->uri);

    if (strcmp(req->uri, "/status.json") == 0) {
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
        httpd_resp_set_hdr(req, "Pragma", "no-cache");

        const uint8_t bufferSize = 16;
        char buffer[bufferSize];
        uint8_t statusVal = 0;
        if (credentialsSaveState != CredentialsSaveState::notSet) {
            switch (credentialsSaveState) {
                case CredentialsSaveState::saved:       statusVal = 1; break;
                case CredentialsSaveState::saveRestart: statusVal = 2; break;
                case CredentialsSaveState::saveError:   statusVal = 3; break;
                default: statusVal = 0; break;
            }
        }
        else {
            switch (credentialsState) {
                case CredentialsState::checking:        statusVal = 4; break;
                case CredentialsState::invalidSSID:     statusVal = 5; break;
                case CredentialsState::invalidPass:     statusVal = 6; break;
                case CredentialsState::processingError: statusVal = 7; break;
                case CredentialsState::checkFail:       statusVal = 8; break;
                case CredentialsState::checkSuccess:    statusVal = 9; break;
                case CredentialsState::checkTimeout:    statusVal = 10; break;
                default: statusVal = 0; break;
            }
        }
        snprintf(buffer, bufferSize, "{\"status\":%d}\n", statusVal);

        httpd_resp_send(req, buffer, strnlen(buffer, bufferSize));
        return ESP_OK;
    }

    if (strcmp(req->uri, "/jquery.js") == 0) {
        httpd_resp_send(req, (const char *)jquery_js_start, jquery_js_end - jquery_js_start);
        return ESP_OK;
    }

    SendMainPage(req);
    return ESP_OK;
}

esp_err_t HTTPConfigServer::HandlePostRequest(httpd_req_t* req)
{
    if (req == nullptr) return ESP_FAIL;

    credentialsState     = CredentialsState::checking;
    credentialsSaveState = CredentialsSaveState::notSet;

    credentialsReceived = false;
    checkThem = false;
    saveThem = false;

    if (strcmp(req->uri, "/check.json") == 0) {
        ESP_LOGI(TAG, "check.json");
        checkThem = true;
    }
    else if (strcmp(req->uri, "/save.json") == 0) {
        ESP_LOGI(TAG, "save.json");
        saveThem = true;
    }
    else{
        // page not found
        credentialsState = CredentialsState::processingError;
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    size_t lenSSID = httpd_req_get_hdr_value_len(req, "X-User-SSID");
    size_t lenPass = httpd_req_get_hdr_value_len(req, "X-User-PASS");

    if (lenSSID == 0 || lenSSID > 31) {
        credentialsState = CredentialsState::invalidSSID;
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid SSID data");
        return ESP_FAIL;
    }
    if (lenPass < 7 || lenPass > 63) {
        credentialsState = CredentialsState::invalidPass;
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid Password data");
        return ESP_FAIL;
    }

    memset(SSID, 0, 32);
    memset(PASS, 0, 64);

    esp_err_t err;
    err = httpd_req_get_hdr_value_str(req, "X-User-SSID", SSID, 32);
    if (err != ESP_OK) {
        credentialsState = CredentialsState::processingError;
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "SSID");
        return ESP_FAIL;
    }
    err = httpd_req_get_hdr_value_str(req, "X-User-PASS", PASS, 64);
    if (err != ESP_OK) {
        credentialsState = CredentialsState::processingError;
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Password");
        return ESP_FAIL;
    }

    credentialsReceived = true;

    if (events != nullptr)
        events->SetBits(xBitCredentialsReceived);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
    httpd_resp_set_hdr(req, "Pragma", "no-cache");
    httpd_resp_sendstr(req, "{\"status\":0}");
    return ESP_OK;
}
