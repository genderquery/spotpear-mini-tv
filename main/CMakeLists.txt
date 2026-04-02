idf_component_register(
    SRCS "main.c" "led.c" "lcd.c"
    PRIV_REQUIRES spi_flash esp_driver_gpio
    INCLUDE_DIRS "."
)
