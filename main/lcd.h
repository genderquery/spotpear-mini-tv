#pragma once

#include "stdint.h"
#include "driver/spi_master.h"
#include "esp_lcd_types.h"

#define LCD_H_RES 128
#define LCD_V_RES 128

#define LCD_SPI_HOST SPI2_HOST 

#define LCD_GPIO_NUM_DC GPIO_NUM_0
#define LCD_GPIO_NUM_CS GPIO_NUM_2
#define LCD_GPIO_NUM_SCLK GPIO_NUM_3
#define LCD_GPIO_NUM_MOSI GPIO_NUM_4
#define LCD_GPIO_NUM_RESET GPIO_NUM_5

typedef uint16_t lcd_color_t;

#define LCD_MAX_TRANSFER_SZ (LCD_H_RES * LCD_V_RES * sizeof(lcd_color_t))

extern esp_err_t lcd_init(esp_lcd_panel_io_handle_t *io_handle, esp_lcd_panel_handle_t *panel_handle);


