#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#include <stdio.h>
#include <string.h>

#include "lcd.h"
#include "led.h"
#include "esp_lcd_panel_ops.h"


static uint8_t s_led_state = 0;

void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    // clang-format off
    printf ("This is %s chip with %d CPU core(s), %s%s%s%s, ",
        CONFIG_IDF_TARGET, chip_info.cores,
        (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
        (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
        (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
        (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");
    // clang-format on

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    // clang-format off
    printf("%" PRIu32 "MB %s flash\n",
        flash_size / (uint32_t)(1024 * 1024),
        (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    // clang-format on

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    ESP_ERROR_CHECK(led_init());

    esp_lcd_panel_io_handle_t lcd_panel_io_handle;
    esp_lcd_panel_handle_t lcd_panel_handle;

    ESP_ERROR_CHECK(lcd_init(&lcd_panel_io_handle, &lcd_panel_handle));

    static uint16_t color_data[LCD_H_RES * LCD_V_RES];

    while (1) {
        memset(color_data, rand(), sizeof(color_data));
        ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(lcd_panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, color_data));

        led_set(s_led_state);
        s_led_state = !s_led_state;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
