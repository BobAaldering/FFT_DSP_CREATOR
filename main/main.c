#include <stdlib.h>

#include "nvs_flash.h"

#include "display_communicator.h"
#include "http_server.h"
#include "wifi_pass.h"

#define OLED_WIDTH (128)
#define OLED_HEIGHT (64)

program_data_t program_data;
SSD1306_t oled_display;

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());

    initialize_oled(OLED_WIDTH, OLED_HEIGHT);
    oled_view_startup("  FFT CREATOR  ", "   Starting...  ", " 2023 (c) bobaa ");

    httpd_handle_t server_handle = NULL;

    start_wifi_connection(SSID_NAME, PASS_NAME);
    start_webserver(server_handle);
}
