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

typedef struct program_data {
    float samples[NUMBER_OF_SAMPLES];
    size_t sample_frequency;

    wave_config_t waves[MAXIMUM_WAVES_LENGTH];
    size_t number_of_waves;

    window_config_t window;
} program_data_t;

extern program_data_t program_data;

extern void start_wifi_connection(const char* ssid_name, const char* pass_name);

extern void start_webserver(httpd_handle_t server_handle);
extern void stop_webserver(httpd_handle_t server_handle);

extern void wifi_event_handler(void*, esp_event_base_t, int32_t event_id, void*);

extern esp_err_t wave_post_handler(httpd_req_t* request);
extern esp_err_t fft_post_handler(httpd_req_t* request);
extern esp_err_t dac_post_handler(httpd_req_t* request);

extern esp_err_t parse_wave_data(const char* json_data);
extern esp_err_t parse_fft_data(const char* json_data);

#endif
