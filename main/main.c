/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"

#include <esp_http_server.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include <string.h>
#include <fcntl.h>
#include "esp_http_server.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "cJSON.h"
#include<stdlib.h>
#include "driver/uart.h"
/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */
#define BLINK_GPIO 23
#define GPIO_INPUT_IO_22     22
#define GPIO_INPUT_IO_19     19
#define GPIO_INPUT_IO_18     18
#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)
#define TXD_PIN2 (GPIO_NUM_16)
#define RXD_PIN2 (GPIO_NUM_17)
static const char *TAG = "example";
static const int RX_BUF_SIZE = 1024;
static const int RX_BUF_SIZE2 = 1024;

int A=0;

/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    gpio_set_level(BLINK_GPIO, 1);

    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-2", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-2: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-1: %s", buf);
        }
        free(buf);
    }

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query1=%s", param);
            }
            if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query3=%s", param);
            }
            if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query2=%s", param);
            }
        }
        free(buf);
    }
    /* UART1 if pin G4 and G5 are shorted the message will be an echo */
     char* data="10";
     char* data2="11";
     int len = strlen(data);
     int len1 = strlen(data2);
    uart_write_bytes(UART_NUM_1, data, len);
    uart_write_bytes(UART_NUM_2, data2, len1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
     uint8_t* data1 = (uint8_t*) malloc(RX_BUF_SIZE+1);
     uint8_t* data3 = (uint8_t*) malloc(RX_BUF_SIZE2+1);
    const int rxBytes = uart_read_bytes(UART_NUM_1, data1, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
    const int rxBytes1 = uart_read_bytes(UART_NUM_2, data3, RX_BUF_SIZE2, 1000 / portTICK_RATE_MS);
    /* UART1 if pin G4 and G5 are shorted the message will be an echo */
    /* Delay waiting for response from blue pill*/
    /* UART2 if pin G16 and G17 are shorted the message will be an echo */


    if (rxBytes > 0) {
            data1[rxBytes] = 0;
            ESP_LOGI(TAG, "Read %d bytes: '%s'", rxBytes, data1);
            ESP_LOG_BUFFER_HEXDUMP(TAG, data1, rxBytes, ESP_LOG_INFO);

          }
          if (rxBytes1 > 0) {
                  data3[rxBytes1] = 0;
                  ESP_LOGI(TAG, "Read %d bytes: '%s'", rxBytes1, data3);
                  ESP_LOG_BUFFER_HEXDUMP(TAG, data3, rxBytes1, ESP_LOG_INFO);

                }
            /* if by this time there has been no response from blue pill asume something went wrong */
    /* After uart transmission send uart response to website */
    /*read pins value
    char str[2]={0};
    char str1[2]={0};
    char str2[2]={0};
    itoa(gpio_get_level(22),str,10);
    itoa(gpio_get_level(19),str1,10);
    itoa(gpio_get_level(18),str2,10);*/
    /* Set some custom headers */
    /* return pin values as headers
    httpd_resp_set_hdr(req, "Custom-Header-1", str );
    httpd_resp_set_hdr(req, "Custom-Header-2", str1 );
    httpd_resp_set_hdr(req, "Custom-Header-3", str2 );
    */
    /* Send response with custom headers and body set as the
     * string passed in user context*/
     char* str1=(char*) data1;
      char* str2=(char*) data3;
     char * str3 = (char *) malloc(1 + strlen(str1)+ strlen(str2) );
     strcpy(str3, str1);
    strcat(str3, str2);
    const char* resp_str1 = (char*) str3;
    httpd_resp_send(req, resp_str1, strlen(resp_str1));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    gpio_set_level(BLINK_GPIO, 0);
    return ESP_OK;
}

static const httpd_uri_t hello = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "hello"
};

/* An HTTP POST handler */
static esp_err_t echo_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }

    // End response

    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t echo = {
    .uri       = "/echo",
    .method    = HTTP_POST,
    .handler   = echo_post_handler,
    .user_ctx  = NULL
};
static esp_err_t light_brightness_post_handler(httpd_req_t *req)
{
    char buf[100];
    int total_len = req->content_len;
    int cur_len = 0;
    int received = 0;
    gpio_set_level(BLINK_GPIO, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(BLINK_GPIO, 0);


    while (cur_len < total_len) {
       received = httpd_req_recv(req, buf + cur_len, total_len);
       if (received <= 0) {
           /* Respond with 500 Internal Server Error */
           httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
           return ESP_FAIL;
       }
       cur_len += received;
   }
   buf[total_len] = '\0';

        /* Log data received */


    cJSON *root = cJSON_Parse(buf);
    int pan = cJSON_GetObjectItem(root, "pan")->valueint;
    int tilt = cJSON_GetObjectItem(root, "tilt")->valueint;
    int energy = cJSON_GetObjectItem(root, "energy")->valueint;
    ESP_LOGI(TAG, "Light control: pan = %d, tilt = %d, energy = %d", pan, tilt, energy);

    cJSON_Delete(root);

    httpd_resp_sendstr(req, "Post control value successfully");

    /* UART1 send data pan and tilt and recieve data */
    char str3[5]={0};
    char str4[5]={0};
    itoa(pan,str3,10);
    itoa(tilt,str4,10);
    char str[80];
    strcpy(str, str3);
    strcat(str, ",");
    strcat(str, str4);
     char* data=str;
     int len = strlen(data);
    uart_write_bytes(UART_NUM_1, data, len);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
     uint8_t* data1 = (uint8_t*) malloc(RX_BUF_SIZE+1);
    const int rxBytes = uart_read_bytes(UART_NUM_1, data1, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
    /* UART1 if pin G4 and G5 are shorted the message will be an echo */
    if (rxBytes > 0) {
            data1[rxBytes] = 0;
            ESP_LOGI(TAG, "Read %d bytes: '%s'", rxBytes, data1);
            ESP_LOG_BUFFER_HEXDUMP(TAG, data1, rxBytes, ESP_LOG_INFO);
        }
    return ESP_OK;

}

static const httpd_uri_t light_brightness_post_uri = {
    .uri       = "/api/v1/light/brightness",
    .method    = HTTP_POST,
    .handler   = light_brightness_post_handler,
    .user_ctx  = NULL
};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/hello", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    } else if (strcmp("/echo", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

/* An HTTP PUT handler. This demonstrates realtime
 * registration and deregistration of URI handlers
 */
static esp_err_t ctrl_put_handler(httpd_req_t *req)
{
    char buf;
    int ret;

    if ((ret = httpd_req_recv(req, &buf, 1)) <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    if (buf == '0') {
        /* URI handlers can be unregistered using the uri string */
        ESP_LOGI(TAG, "Unregistering /hello and /echo URIs");
        httpd_unregister_uri(req->handle, "/hello");
        httpd_unregister_uri(req->handle, "/echo");
        httpd_unregister_uri(req->handle, "/api/v1/light/brightness");

        /* Register the custom error handler */
        httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, http_404_error_handler);
    }
    else {
        ESP_LOGI(TAG, "Registering /hello and /echo URIs");
        httpd_register_uri_handler(req->handle, &hello);
        httpd_register_uri_handler(req->handle, &echo);
        httpd_register_uri_handler(req->handle, &light_brightness_post_uri);
        /* Unregister custom error handler */
        httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, NULL);
    }

    /* Respond with empty body */
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t ctrl = {
    .uri       = "/ctrl",
    .method    = HTTP_PUT,
    .handler   = ctrl_put_handler,
    .user_ctx  = NULL
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &echo);
        httpd_register_uri_handler(server, &ctrl);
        httpd_register_uri_handler(server, &light_brightness_post_uri);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}


void app_main(void)
{
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_pad_select_gpio(GPIO_INPUT_IO_22);
    gpio_pad_select_gpio(GPIO_INPUT_IO_19);
    gpio_pad_select_gpio(GPIO_INPUT_IO_18);

    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_INPUT_IO_22 , GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_INPUT_IO_19 , GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_INPUT_IO_18 , GPIO_MODE_INPUT);

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,
     * and re-start it upon connection.
     *uart1 setup
     */
     const uart_config_t uart_config = {
             .baud_rate = 115200,
             .data_bits = UART_DATA_8_BITS,
             .parity = UART_PARITY_DISABLE,
             .stop_bits = UART_STOP_BITS_1,
             .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
         };
         uart_param_config(UART_NUM_1, &uart_config);
         uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
         // We won't use a buffer for sending data.
         uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
         /* UART2 setup
          */

         uart_param_config(UART_NUM_2, &uart_config);
         uart_set_pin(UART_NUM_2, TXD_PIN2, RXD_PIN2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
         // We won't use a buffer for sending data.
         uart_driver_install(UART_NUM_2, RX_BUF_SIZE2 * 2, 0, 0, NULL, 0);

#ifdef CONFIG_EXAMPLE_CONNECT_WIFI
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_WIFI
#ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_ETHERNET

    /* Start the server for the first time */
    server = start_webserver();
}
