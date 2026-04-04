#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "lcd.h"
#include "led.h"
#include "esp_lcd_panel_ops.h"

#include "lvgl.h"
#include "esp_lvgl_port.h"

static uint8_t s_led_state = 0;

void app_main(void)
{
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

    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

    const lvgl_port_display_cfg_t disp_cfg = {
          .io_handle = lcd_panel_io_handle,
          .panel_handle = lcd_panel_handle,
          .buffer_size = LCD_H_RES*LCD_V_RES,
          .double_buffer = true,
          .hres = LCD_H_RES,
          .vres = LCD_V_RES,
          .monochrome = false,
          .color_format = LV_COLOR_FORMAT_RGB565,
          .rounder_cb = NULL,
          .rotation = {
              .swap_xy = false,
              .mirror_x = false,
              .mirror_y = false,
          },
          .flags = {
              .buff_dma = true,
              .swap_bytes = true,
          }
      };

    lv_disp_t *disp_handle = lvgl_port_add_disp(&disp_cfg);

    lvgl_port_lock(0);
    lv_obj_t *screen = lv_disp_get_scr_act(disp_handle);
    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "Hello, world!");
    lvgl_port_unlock();

    while (1) {
        led_set(s_led_state);
        s_led_state = !s_led_state;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
