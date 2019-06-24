#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"

extern int main();
extern void test_spiffs();

#define BLINK_GPIO 2

void blink()
{
    while(1) {
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(980 / portTICK_PERIOD_MS);
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

static const char *TAG = "spiffs";
void spiffs_init(){
    ESP_LOGI(TAG, "Initializing SPIFFS");
    
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };
    
    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%d)", ret);
        }
        return;
    }
    
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information");
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    
}

void spiffs_listdir(const char* path)
{
    char name_dir_file[64];
    printf("Spiffs.Dir.Info\n");
    unlink(name_dir_file);
    DIR* dir = opendir(path);
    while (true) {
        struct dirent* de = readdir(dir);
        if (!de) break;
        printf(" %s\n", de->d_name); 
    }
    printf("\n");
    unlink(name_dir_file);
}

void app_main(void)
{
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 1);
    printf("Start\n");
    
	main();
    
    gpio_set_level(BLINK_GPIO, 0);
    printf("Stop\n");
    blink(); // Stop
}
