#include <stdlib.h>

#include "nvs_flash.h"

#include "dac_communicator.h"
#include "display_communicator.h"
#include "http_server.h"
#include "wifi_pass.h"

#define OLED_WIDTH (128)
#define OLED_HEIGHT (64)

// Instantiate the 'dac_data' structure, with all its initial values:
dac_data_t dac_data = {
    .digital_samples = NULL, 
    .number_of_samples = 0,
    .timer = NULL
};

// Instantiate the 'program_data' structure, with all its initial values:
program_data_t program_data = {
    .samples = {0.0},
    .sample_frequency = 0,
    .waves = {},
    .number_of_waves = 0,
    .window = 0,
    .prevent_dac_overflow = false
};

SSD1306_t oled_display; // Instantiate the 'oled_display' structure.

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init()); // Initialize the 'NVS Flash' for storing Wi-Fi communication data.

    initialize_oled(OLED_WIDTH, OLED_HEIGHT);                                 // Initialize the OLED display.
    ESP_ERROR_CHECK(oled_view_startup("  FFT CREATOR  ", " 2023 (c) bobaa")); // Show a startup screen on OLED display.

    httpd_handle_t server_handle = NULL; // An HTTP server handle.

    start_wifi_connection(SSID_NAME, PASS_NAME); // Start the Wi-Fi connection.
    start_webserver(server_handle);              // Start the web server.
}
