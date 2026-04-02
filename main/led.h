#pragma once

#include "esp_err.h"
#include "stdint.h"

#define LED_GPIO GPIO_NUM_11

extern esp_err_t led_init(void);
extern esp_err_t led_set(uint32_t level);
