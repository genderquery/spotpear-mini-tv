#include "driver/spi_common.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_st7735.h"
#include "esp_lcd_types.h"
#include "stdint.h"

#include "lcd.h"

#define TAG "lcd"

esp_err_t lcd_init(esp_lcd_panel_io_handle_t *io_handle, esp_lcd_panel_handle_t *panel_handle)
{
    esp_err_t ret = ESP_OK;

    spi_bus_config_t bus_config = {
        .sclk_io_num = LCD_GPIO_NUM_SCLK,
        .mosi_io_num = LCD_GPIO_NUM_MOSI,
        .miso_io_num = GPIO_NUM_NC,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = LCD_MAX_TRANSFER_SZ,
    };

    ESP_GOTO_ON_ERROR(spi_bus_initialize(LCD_SPI_HOST, &bus_config, SPI_DMA_CH_AUTO), err, TAG, "SPI bus init failed");

    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = LCD_GPIO_NUM_DC,
        .cs_gpio_num = LCD_GPIO_NUM_CS,
        .pclk_hz = 40 * 1000 * 1000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };

    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_SPI_HOST, &io_config, io_handle), err, TAG, "Panel IO init failed");

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LCD_GPIO_NUM_RESET,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };

    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_st7735(*io_handle, &panel_config, panel_handle), err, TAG, "ST7735 driver init failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_reset(*panel_handle), err, TAG, "Panel reset failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_init(*panel_handle), err, TAG, "Panel init failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_invert_color(*panel_handle, false), err, TAG, "Invert color failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_set_gap(*panel_handle, 2, 1), err, TAG, "Set gap failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_disp_on_off(*panel_handle, true), err, TAG, "Turn on display failed");

    return ret;

err:

    if (panel_handle != NULL) {
        esp_lcd_panel_del(*panel_handle);
        panel_handle = NULL;
    }

    if (io_handle != NULL) {
        esp_lcd_panel_io_del(*io_handle);
        io_handle = NULL;
    }

    spi_bus_free(LCD_SPI_HOST);

    return ret;
}
