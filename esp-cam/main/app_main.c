// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../components/http_server/my_http_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "camera.h"
#include "bitmap.h"
//#include "led.h"
//#include "qr_recoginize.h"
#include <lwip/sockets.h>

void setup(camera_config_t camera_config);
void setup_http(camera_config_t camera_config);
void setup_camera(camera_config_t camera_config);
void dumy_disp();
static void handle_grayscale_pgm(http_context_t http_ctx, void* ctx);
static void handle_rgb_bmp(http_context_t http_ctx, void* ctx);
static void handle_rgb_bmp_stream(http_context_t http_ctx, void* ctx);
static void handle_jpg(http_context_t http_ctx, void* ctx);
static void handle_jpg_stream(http_context_t http_ctx, void* ctx);
static esp_err_t event_handler(void *ctx, system_event_t *event);
static void initialise_wifi(void);

static const char* TAG = "cam_msg";

static const char* STREAM_CONTENT_TYPE =
        "multipart/x-mixed-replace; boundary=123456789000000000000987654321";

static const char* STREAM_BOUNDARY = "--123456789000000000000987654321";

EventGroupHandle_t s_wifi_event_group;
static const int CONNECTED_BIT = BIT0;
static ip4_addr_t s_ip_addr;
static camera_pixelformat_t s_pixel_format;

#define CAMERA_PIXEL_FORMAT CAMERA_PF_GRAYSCALE // CAMERA_PF_JPEG
#define CAMERA_FRAME_SIZE CAMERA_FS_QQVGA // CAMERA_FS_QVGA // CAMERA_FS_SVGA
//#define CAMERA_PIXEL_FORMAT CAMERA_PF_JPEG
//#define CAMERA_FRAME_SIZE CAMERA_FS_SVGA
#define LED_GPIO 2
#define TRIG_GPIO 15

    camera_config_t camera_config = {
        //.ledc_channel = LEDC_CHANNEL_0, .ledc_timer = LEDC_TIMER_0,
        .pin_d0 = CONFIG_D0, .pin_d1 = CONFIG_D1, .pin_d2 = CONFIG_D2, .pin_d3 = CONFIG_D3,
        .pin_d4 = CONFIG_D4, .pin_d5 = CONFIG_D5, .pin_d6 = CONFIG_D6, .pin_d7 = CONFIG_D7,
        .pin_xclk     = CONFIG_XCLK,  .pin_pclk     = CONFIG_PCLK,
        .pin_vsync    = CONFIG_VSYNC, .pin_href     = CONFIG_HREF,
        .pin_sscb_sda = CONFIG_SDA,   .pin_sscb_scl = CONFIG_SCL,
        .pin_reset    = CONFIG_RESET, .xclk_freq_hz = CONFIG_XCLK_FREQ,
    };
    int camera_trig_mode;

void app_main(){
	setup(camera_config);
    
 	//while(1){ camera_run(); dumy_disp(); }
}

void setup(camera_config_t camera_config)
{
    gpio_set_direction(TRIG_GPIO, GPIO_MODE_INPUT);
    gpio_pullup_en(TRIG_GPIO);
    camera_trig_mode = gpio_get_level(TRIG_GPIO);
    
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO, 1);

    esp_log_level_set("wifi", ESP_LOG_WARN);
    esp_log_level_set("gpio", ESP_LOG_WARN);
    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ESP_ERROR_CHECK( nvs_flash_init() );
    }


	setup_camera(camera_config);

    gpio_set_level(LED_GPIO, 0);
//    databuf = (char *) malloc(BUF_SIZE);
    initialise_wifi();

    gpio_set_level(LED_GPIO, 1);
	setup_http(camera_config);

    ESP_LOGI(TAG, "Free heap: %u", xPortGetFreeHeapSize());
    ESP_LOGI(TAG, "Camera demo ready");
    gpio_set_level(LED_GPIO, 0);
    
}
void setup_camera(camera_config_t camera_config){
	
    camera_model_t camera_model;
    esp_err_t err = camera_probe(&camera_config, &camera_model);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera probe failed with error 0x%x", err);
        return;
    }

    if (camera_model == CAMERA_OV7725) {
        s_pixel_format = CAMERA_PIXEL_FORMAT;
        camera_config.frame_size = CAMERA_FRAME_SIZE;
        ESP_LOGI(TAG, "Detected OV7725 camera, using %s bitmap format",
                CAMERA_PIXEL_FORMAT == CAMERA_PF_GRAYSCALE ?
                        "grayscale" : "RGB565");
    } else if (camera_model == CAMERA_OV2640) {
        ESP_LOGI(TAG, "Detected OV2640 camera, using JPEG format");
        s_pixel_format = CAMERA_PIXEL_FORMAT;
        camera_config.frame_size = CAMERA_FRAME_SIZE;
        if (s_pixel_format == CAMERA_PF_JPEG)
        camera_config.jpeg_quality = 15;
    } else {
        ESP_LOGE(TAG, "Camera not supported");
        return;
    }

    camera_config.pixel_format = s_pixel_format;
    err = camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        return;
    }
}

void setup_http(camera_config_t camera_config){
    http_server_t server;
    http_server_options_t http_options = HTTP_SERVER_OPTIONS_DEFAULT();
    ESP_ERROR_CHECK( http_server_start(&http_options, &server) );

    if (s_pixel_format == CAMERA_PF_GRAYSCALE) {
        ESP_ERROR_CHECK( http_register_handler(server, "/pgm", HTTP_GET, HTTP_HANDLE_RESPONSE, &handle_grayscale_pgm, &camera_config) );
        ESP_LOGI(TAG, "Open http://" IPSTR "/pgm for a single image/x-portable-graymap image", IP2STR(&s_ip_addr));
    }
    if (s_pixel_format == CAMERA_PF_RGB565) {
        ESP_ERROR_CHECK( http_register_handler(server, "/bmp", HTTP_GET, HTTP_HANDLE_RESPONSE, &handle_rgb_bmp, NULL) );
        ESP_LOGI(TAG, "Open http://" IPSTR "/bmp for single image/bitmap image", IP2STR(&s_ip_addr));
        ESP_ERROR_CHECK( http_register_handler(server, "/bmp_stream", HTTP_GET, HTTP_HANDLE_RESPONSE, &handle_rgb_bmp_stream, NULL) );
        ESP_LOGI(TAG, "Open http://" IPSTR "/bmp_stream for multipart/x-mixed-replace stream of bitmaps", IP2STR(&s_ip_addr));
    }
    if (s_pixel_format == CAMERA_PF_JPEG) {
        ESP_ERROR_CHECK( http_register_handler(server, "/jpg", HTTP_GET, HTTP_HANDLE_RESPONSE, &handle_jpg, NULL) );
        ESP_LOGI(TAG, "Open http://" IPSTR "/jpg for single image/jpg image", IP2STR(&s_ip_addr));
        ESP_ERROR_CHECK( http_register_handler(server, "/jpg_stream", HTTP_GET, HTTP_HANDLE_RESPONSE, &handle_jpg_stream, NULL) );
        ESP_LOGI(TAG, "Open http://" IPSTR "/jpg_stream for multipart/x-mixed-replace stream of JPEGs", IP2STR(&s_ip_addr));
    }
}
static esp_err_t write_frame(http_context_t http_ctx)
{
    http_buffer_t fb_data = {
            .data = camera_get_fb(),
            .size = camera_get_data_size(),
            .data_is_persistent = true
    };
    return http_response_write(http_ctx, &fb_data);
}

static void handle_grayscale_pgm(http_context_t http_ctx, void* ctx)
{
    int trig = gpio_get_level(TRIG_GPIO); // Normal 1, Push 0
    gpio_set_level(LED_GPIO, 1);

    esp_err_t err = camera_run();
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Camera capture failed with error = %d", err);
        return;
    }
    
    char* pgm_header_str;
    asprintf(&pgm_header_str, "P5 %d %d %d\n",
            camera_get_fb_width(), camera_get_fb_height(), 254+trig); // "P5 160 120 255\n" = 15byte
    if (pgm_header_str == NULL) {
        return;
    }

    size_t response_size = strlen(pgm_header_str) + camera_get_data_size();
    http_response_begin(http_ctx, 200, "image/x-portable-graymap", response_size);
    http_response_set_header(http_ctx, "Content-disposition", "inline; filename=capture.pgm");
    http_buffer_t pgm_header = { .data = pgm_header_str };
    http_response_write(http_ctx, &pgm_header);
    free(pgm_header_str);

    write_frame(http_ctx);
    http_response_end(http_ctx);
//    ESP_LOGI(TAG, "Free heap: %u", xPortGetFreeHeapSize());
//#if CONFIG_QR_RECOGNIZE
//    camera_config_t *camera_config = ctx;
//    xTaskCreate(qr_recoginze, "qr_recoginze", 111500, camera_config, 5, NULL);
//#endif
    gpio_set_level(LED_GPIO, 0);
}

void dumy_disp(){
    char* aa16 = "  ..::--++==##%%";
    //printf("\033[1;1H");
    //printf("[%d %d %d]\n", camera_get_fb_width(), camera_get_fb_height(), camera_get_data_size());
   	uint8_t* bdp = camera_get_fb();
    uint8_t bd;
   	for(int j=0; j<120; j=j+4){
	    for(int i=0; i<160; i=i+2){
            bd = bdp[j*160+i];
            bd = aa16[bd >> 4];
            printf("%c", bd);
        }
    	printf("\n");
    }
}

static void handle_rgb_bmp(http_context_t http_ctx, void* ctx)
{
    esp_err_t err = camera_run();
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Camera capture failed with error = %d", err);
        return;
    }

    bitmap_header_t* header = bmp_create_header(camera_get_fb_width(), camera_get_fb_height());
    if (header == NULL) {
        return;
    }

    http_response_begin(http_ctx, 200, "image/bmp", sizeof(*header) + camera_get_data_size());
    http_buffer_t bmp_header = {
            .data = header,
            .size = sizeof(*header)
    };
    http_response_set_header(http_ctx, "Content-disposition", "inline; filename=capture.bmp");
    http_response_write(http_ctx, &bmp_header);
    free(header);

    write_frame(http_ctx);
    http_response_end(http_ctx);
}

static void handle_jpg(http_context_t http_ctx, void* ctx)
{
	//if(get_light_state()) led_open();
    gpio_set_level(LED_GPIO, 1);
    esp_err_t err = camera_run();
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Camera capture failed with error = %d", err);
        return;
    }

    http_response_begin(http_ctx, 200, "image/jpeg", camera_get_data_size());
    http_response_set_header(http_ctx, "Content-disposition", "inline; filename=capture.jpg");
    write_frame(http_ctx);
    http_response_end(http_ctx);
    //led_close();
    gpio_set_level(LED_GPIO, 0);
}


static void handle_rgb_bmp_stream(http_context_t http_ctx, void* ctx)
{
    http_response_begin(http_ctx, 200, STREAM_CONTENT_TYPE, HTTP_RESPONSE_SIZE_UNKNOWN);
    bitmap_header_t* header = bmp_create_header(camera_get_fb_width(), camera_get_fb_height());
    if (header == NULL) {
        return;
    }
    http_buffer_t bmp_header = {
            .data = header,
            .size = sizeof(*header)
    };


    while (true) {
        esp_err_t err = camera_run();
        if (err != ESP_OK) {
            ESP_LOGD(TAG, "Camera capture failed with error = %d", err);
            return;
        }

        err = http_response_begin_multipart(http_ctx, "image/bitmap",
                camera_get_data_size() + sizeof(*header));
        if (err != ESP_OK) {
            break;
        }
        err = http_response_write(http_ctx, &bmp_header);
        if (err != ESP_OK) {
            break;
        }
        err = write_frame(http_ctx);
        if (err != ESP_OK) {
            break;
        }
        err = http_response_end_multipart(http_ctx, STREAM_BOUNDARY);
        if (err != ESP_OK) {
            break;
        }
    }

    free(header);
    http_response_end(http_ctx);
}

static void handle_jpg_stream(http_context_t http_ctx, void* ctx)
{
    http_response_begin(http_ctx, 200, STREAM_CONTENT_TYPE, HTTP_RESPONSE_SIZE_UNKNOWN);
    //if(get_light_state()) led_open();
    while (true) {
        esp_err_t err = camera_run();
        if (err != ESP_OK) {
            ESP_LOGD(TAG, "Camera capture failed with error = %d", err);
            return;
        }
        err = http_response_begin_multipart(http_ctx, "image/jpg",
                camera_get_data_size());
        if (err != ESP_OK) {
            break;
        }
        err = write_frame(http_ctx);
        if (err != ESP_OK) {
            break;
        }
        err = http_response_end_multipart(http_ctx, STREAM_BOUNDARY);
        if (err != ESP_OK) {
            break;
        }
    }
    http_response_end(http_ctx);
    //led_close();
}


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
            s_ip_addr = event->event_info.got_ip.ip_info.ip;
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            esp_wifi_connect();
            xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}
static void initialise_wifi(void)
{
    tcpip_adapter_init();
    
    if(strlen(CONFIG_WIFI_IP) != 0){
	    tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA);
	    tcpip_adapter_ip_info_t ipInfo;
	    inet_pton(AF_INET, CONFIG_WIFI_IP, &ipInfo.ip); 
	    inet_pton(AF_INET, CONFIG_WIFI_GW, &ipInfo.gw); 
	    inet_pton(AF_INET, CONFIG_WIFI_MASK, &ipInfo.netmask); 
	    tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo); 
    }
    
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    //ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid     = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
        },
    };
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_set_ps(WIFI_PS_NONE) );
    ESP_LOGI(TAG, "Connecting to \"%s\"", wifi_config.sta.ssid);
    xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected");
}
