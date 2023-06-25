#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <string.h>
#include <cJSON.h>

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_http_server.h"

#include "dac_communicator.h"
#include "fft_transform.h"
#include "wave_transform.h"
#include "window_transform.h"

#define WIFI_SERVER_TAG ("WIFI_SERVER_H_")

#define MAXIMUM_CONTENT_LENGTH (250)
#define MAXIMUM_WAVES_LENGTH (10)

#define NUMBER_OF_SAMPLES (2048)

/// @brief Defining a struct called `program_data`, that contains all the needed data for running the HTTP server (the actual program - completely event based).
typedef struct program_data {
    float samples[NUMBER_OF_SAMPLES]; // This field is an array of `float` samples.
    size_t sample_frequency;          // This field contains a `size_t` with the sample frequency.

    wave_config_t waves[MAXIMUM_WAVES_LENGTH]; // This field contains an array of `wave_config_t` waves.
    size_t number_of_waves;                    // This field contains a `size_t` with the number of waves.

    window_config_t window; // This field represents a `window_config_t` window.

    bool prevent_dac_overflow; // Field with a boolean flag to prevent DAC overflow.
} program_data_t;

extern program_data_t program_data;

/// @brief This function starts a Wi-Fi connection with the provided SSID and password.
/// @param ssid_name The name of the Wi-Fi network (also known as the SSID) that you want to connect to.
/// @param pass_name The password of the Wi-Fi network that you want to connect to.
extern void start_wifi_connection(const char* ssid_name, const char* pass_name);

/// @brief This function starts a web server and registers URI handlers for POST requests to `/wave`, `/fft`, and `/dac`.
/// @param server_handle A handle to the HTTP server instance that is being started.
extern void start_webserver(httpd_handle_t server_handle);

/// @brief This function stops a webserver.
/// @param server_handle A handle to the HTTP server instance that is being stopped.
extern void stop_webserver(httpd_handle_t server_handle);

/// @brief This function handles different Wi-Fi events and logs messages accordingly while also displaying them on an OLED screen.
/// @param _ A pointer to user data (not used in this function).
/// @param _ A pointer to user data (not used in this function).
/// @param event_id The ID of the Wi-Fi event that occurred. This is used in the switch statement to determine which action to take based on the event.
/// @param _ A pointer to user data (not used in this function).
extern void wifi_event_handler(void*, esp_event_base_t, int32_t event_id, void*);

/// @brief This function handles a POST request for generating waves and sends a response indicating successful execution.
/// @param request A pointer to the HTTP request structure, which contains information about the incoming HTTP request.
/// @return An `esp_err_t` value, which is either `ESP_OK` if the function executes successfully or `ESP_FAIL` if there is an error.
extern esp_err_t wave_post_handler(httpd_req_t* request);

/// @brief This function handles a POST request for FFT data, parses the data, applies FFT on it, and sends a response.
/// @param request A pointer to the HTTP request structure, which contains information about the incoming HTTP request.
/// @return An `esp_err_t` value, which is either `ESP_OK` if the function executes successfully or `ESP_FAIL` if there is an error.
extern esp_err_t fft_post_handler(httpd_req_t* request);

/// @brief This function handles a POST request for a DAC output and sends a response indicating successful execution.
/// @param request A pointer to the HTTP request structure, which contains information about the incoming HTTP request.
/// @return An `esp_err_t` value, which is either `ESP_OK` if the function executes successfully or `ESP_FAIL` if there is an error.
extern esp_err_t dac_post_handler(httpd_req_t* request);

/// @brief This function parses JSON data containing wave information and stores it in a program data structure.
/// @param json_data A string containing JSON data to be parsed.
/// @return An `esp_err_t` value, which is either `ESP_OK` if the function executes successfully or `ESP_FAIL` if there is an error.
extern esp_err_t parse_wave_data(const char* json_data);

/// @brief This function parses JSON data and extracts a window configuration value from it.
/// @param json_data A string containing JSON data to be parsed.
/// @return An `esp_err_t` value, which is either `ESP_OK` if the function executes successfully or `ESP_FAIL` if there is an error.
extern esp_err_t parse_fft_data(const char* json_data);

/// @brief The function parses a JSON string and extracts a boolean value to set a flag in a program's data structure.
/// @param json_data A string containing JSON data to be parsed.
/// @return An `esp_err_t` value, which is either `ESP_OK` if the function executes successfully or `ESP_FAIL` if there is an error.
extern esp_err_t parse_dac_data(const char* json_data);

#endif
