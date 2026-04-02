#include "esp_err.h"
#include "esp_check.h"
#include "stdint.h"
#include "driver/gpio.h"
#include "led.h"

#define TAG "led"

esp_err_t led_init(void)
{
    ESP_RETURN_ON_ERROR(gpio_reset_pin(LED_GPIO), TAG, "reset");
    ESP_RETURN_ON_ERROR(gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT), TAG, "set dir");

    return ESP_OK;
}

esp_err_t led_set(uint32_t level)
{
    return gpio_set_level(LED_GPIO, level);
}
