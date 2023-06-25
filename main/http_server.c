#include "http_server.h"

void start_wifi_connection(const char* ssid_name, const char* pass_name) {
    ESP_ERROR_CHECK(esp_netif_init());                // Initialize the network interface layer.
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Create the default event loop.
    esp_netif_create_default_wifi_sta();              // Create the default Wi-Fi station interface.

    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT(); // Set up the Wi-Fi initialization configuration with default values.

    ESP_ERROR_CHECK(esp_wifi_init(&wifi_initiation));                                                     // Initialize the Wi-Fi driver with the configuration.
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));  // Register the Wi-Fi event handler to handle all Wi-Fi related events.
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL)); // Register the Wi-Fi event handler to handle the event of obtaining an IP-address.

    // Create a Wi-Fi configuration structure and fill it with the provided SSID and password:
    wifi_config_t wifi_configuration_data = {};

    strcpy((char*)wifi_configuration_data.sta.ssid, ssid_name);
    strcpy((char*)wifi_configuration_data.sta.password, pass_name);
    
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration_data)); // Set the Wi-Fi station configuration with the provided SSID and password.
    ESP_ERROR_CHECK(esp_wifi_start());                                           // Start the Wi-Fi driver.
    ESP_ERROR_CHECK(esp_wifi_connect());                                         // Initiate a connection to the configured Wi-Fi network
}

void start_webserver(httpd_handle_t server_handle) {
    httpd_config_t http_configuration = HTTPD_DEFAULT_CONFIG(); // Create the default HTTP server configuration.

    ESP_ERROR_CHECK(httpd_start(&server_handle, &http_configuration)); // Start the HTTP server with the provided server handle and configuration.

    // Define the URI and corresponding handler for the `/wave` endpoint:
    httpd_uri_t wave_uri = {
        .uri = "/wave",
        .method = HTTP_POST,
        .handler = wave_post_handler,
        .user_ctx = NULL
    };

    // Define the URI and corresponding handler for the `/fft` endpoint:
    httpd_uri_t fft_uri = {
        .uri = "/fft",
        .method = HTTP_POST,
        .handler = fft_post_handler,
        .user_ctx = NULL
    };

    // Define the URI and corresponding handler for the `/dac` endpoint:
    httpd_uri_t dac_uri = {
        .uri = "/dac",
        .method = HTTP_POST,
        .handler = dac_post_handler,
        .user_ctx = NULL
    };

    // Register the URI handlers with the HTTP server:
    httpd_register_uri_handler(server_handle, &wave_uri);
    httpd_register_uri_handler(server_handle, &fft_uri);
    httpd_register_uri_handler(server_handle, &dac_uri);

    ESP_LOGI(WIFI_SERVER_TAG, "The webserver with all the URI handlers is started!");
}

void stop_webserver(httpd_handle_t server_handle) {
    ESP_ERROR_CHECK(httpd_stop(server_handle)); // Stop the HTTP server.

    ESP_LOGI(WIFI_SERVER_TAG, "The webserver is no longer active!");
}

void wifi_event_handler(void*, esp_event_base_t, int32_t event_id, void*) {
    switch(event_id) {
        // Handle the event when the Wi-Fi connection on the ESP32 is started:
        case WIFI_EVENT_STA_START:
            ESP_LOGI(WIFI_SERVER_TAG, "Connecting to your Wi-Fi network...");
            ESP_ERROR_CHECK(oled_view_info("Starting Wi-Fi!"));

            break;

        // Handle the event when the ESP32 is connected to a network:
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(WIFI_SERVER_TAG, "Connected to your Wi-Fi network!");
            ESP_ERROR_CHECK(oled_view_info("  Got Wi-Fi!"));

            break;

        // Handle the event when the ESP32 is disconnected from a network:
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGE(WIFI_SERVER_TAG, "Lost your Wi-Fi connection!");
            ESP_ERROR_CHECK(oled_view_error("  Lost Wi-Fi!"));

            break;

        // Handle the event when the ESP32 has obtained an IP-address:
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(WIFI_SERVER_TAG, "We got an IP-adress!");
            ESP_ERROR_CHECK(oled_view_info("Got IP-address!"));

            break;

        // Handle any other event that is not explicitly handled:
        default:
            ESP_LOGE(WIFI_SERVER_TAG, "No ID of the event found! Please try again...");
            ESP_ERROR_CHECK(oled_view_error(" No ID found!"));

            break;
    }
}

esp_err_t wave_post_handler(httpd_req_t* request) {
    char content[MAXIMUM_CONTENT_LENGTH] = {};

    int return_length = httpd_req_recv(request, content, sizeof(content) / sizeof(content[0])); // Receive the content of the HTTP POST request.

     // Check if an error occurred or the request timed out:
    if (return_length <= 0) {
        if (return_length == HTTPD_SOCK_ERR_TIMEOUT)
            httpd_resp_send_408(request);

        return ESP_FAIL;
    }

    ESP_LOGI(WIFI_SERVER_TAG, "The 'wave_post_handler' function is invoked, with content '%s'", content);

    ESP_ERROR_CHECK(oled_view_info("Call to 'wave'!")); // Display an informational message on the OLED.

    ESP_ERROR_CHECK(parse_wave_data(content)); // Parse the wave data from the content.

    // Reset the sampled data.
    for (int i = 0; i < NUMBER_OF_SAMPLES; i++)
        program_data.samples[i] = 0;

    ESP_ERROR_CHECK(generate_waves_f32(program_data.waves, program_data.samples, NUMBER_OF_SAMPLES, program_data.number_of_waves)); // Generate the waveforms based on the wave data.

    // Send a response indicating successful execution of the function.
    const char* response = "Successful execution of the function 'wave_post_handler'!\n";
    httpd_resp_send(request, response, strlen(response));

    return ESP_OK;
}

esp_err_t fft_post_handler(httpd_req_t* request) {
    char content[MAXIMUM_CONTENT_LENGTH] = {};

    int return_length = httpd_req_recv(request, content, sizeof(content) / sizeof(content[0])); // Receive the content of the HTTP POST request.

    // Check if an error occurred or the request timed out:
    if (return_length <= 0) {
        if (return_length == HTTPD_SOCK_ERR_TIMEOUT)
            httpd_resp_send_408(request);

        return ESP_FAIL;
    }

    ESP_LOGI(WIFI_SERVER_TAG, "The 'fft_post_handler' function is invoked, with content '%s'", content);

    ESP_ERROR_CHECK(oled_view_info("Call to 'fft'!")); // Display an informational message on the OLED.

    ESP_ERROR_CHECK(parse_fft_data(content)); // Parse the FFT data from the content.

    fft_data_t data_for_fft = {}; // Create an instance of `fft_data_t`.

    ESP_ERROR_CHECK(initialize_fft_f32(&data_for_fft));                                                                                         // Initialize the FFT module.
    ESP_ERROR_CHECK(apply_fft_f32(&data_for_fft, program_data.samples, program_data.window, NUMBER_OF_SAMPLES, program_data.sample_frequency)); // Apply FFT on the sample data using the initialized FFT module.
    ESP_ERROR_CHECK(de_initialize_fft_f32(&data_for_fft));                                                                                      // De-initialize the FFT module.

    // Send a response indicating successful execution of the function:
    const char* response = "Successful execution of the function 'fft_post_handler'!\n";
    httpd_resp_send(request, response, strlen(response));

    return ESP_OK;
}

esp_err_t dac_post_handler(httpd_req_t* request) {
    char content[MAXIMUM_CONTENT_LENGTH] = {};

    int return_length = httpd_req_recv(request, content, sizeof(content) / sizeof(content[0])); // Receive the content of the HTTP POST request.

    // Receive the content of the HTTP POST request:
    if (return_length <= 0) {
        if (return_length == HTTPD_SOCK_ERR_TIMEOUT)
            httpd_resp_send_408(request);

        return ESP_FAIL;
    }

    ESP_LOGI(WIFI_SERVER_TAG, "The 'dac_post_handler' function is invoked, with content '%s'", content);

    ESP_ERROR_CHECK(oled_view_info("Call to 'dac'!")); // Display an informational message on the OLED.

    ESP_ERROR_CHECK(parse_dac_data(content)); // Parse the DAC data from the content.

    // Set the DAC data values and configurations:
    dac_data.digital_samples = program_data.samples;
    dac_data.number_of_samples = NUMBER_OF_SAMPLES;

    dac_data.prevent_dac_overflow_conversion = program_data.prevent_dac_overflow;
    
    ESP_ERROR_CHECK(dac_output_values(program_data.sample_frequency)); // Output the DAC values with the specified sample frequency.

    // Send a response indicating successful execution of the function:
    const char* response = "Successful execution of the function 'dac_post_handler'!\n";
    httpd_resp_send(request, response, strlen(response));

    return ESP_OK;
}

esp_err_t parse_wave_data(const char* json_data) {
    cJSON* root = cJSON_Parse(json_data);

    // Failed to parse the JSON data:
    if (root == NULL) {
        ESP_LOGE(WIFI_SERVER_TAG, "Failed to parse JSON data!");

        return ESP_FAIL;
    }

    cJSON* sample_frequency_item = cJSON_GetObjectItem(root, "sample_frequency");

    // Check if `sample_frequency` item exists and is a number:
    if (cJSON_IsNumber(sample_frequency_item)) 
        program_data.sample_frequency = sample_frequency_item->valueint;

    cJSON* waves_array = cJSON_GetObjectItem(root, "waves");

    // Check if `waves` item exists and is an array:
    if (!cJSON_IsArray(waves_array)) {
        ESP_LOGE(WIFI_SERVER_TAG, "Invalid waves array in JSON data!");

        cJSON_Delete(root);

        return ESP_FAIL;
    }

    int wave_count = cJSON_GetArraySize(waves_array);

    // Truncate the wave count if it exceeds the maximum supported waves:
    if (wave_count > MAXIMUM_WAVES_LENGTH) {
        ESP_LOGW(WIFI_SERVER_TAG, "Received more waves than supported. Truncating it to '%d' waves!", MAXIMUM_WAVES_LENGTH);

        wave_count = MAXIMUM_WAVES_LENGTH;
    }

    for (int i = 0; i < wave_count; i++) {
        cJSON* wave_item = cJSON_GetArrayItem(waves_array, i);
        cJSON* amplitude_item = cJSON_GetObjectItem(wave_item, "amplitude");
        cJSON* frequency_item = cJSON_GetObjectItem(wave_item, "frequency");
        cJSON* phase_item = cJSON_GetObjectItem(wave_item, "phase");
        cJSON* offset_item = cJSON_GetObjectItem(wave_item, "offset");

        // Check if all required wave properties exist and are numbers:
        if (cJSON_IsNumber(amplitude_item) && cJSON_IsNumber(frequency_item) && cJSON_IsNumber(phase_item) && cJSON_IsNumber(offset_item)) {
            float frequency = (float)frequency_item->valuedouble;
            float absolute_frequency = frequency / program_data.sample_frequency;

            // Check if the absolute frequency is valid:
            if (absolute_frequency <= 1.0f) {
                program_data.waves[i].amplitude = (float)amplitude_item->valuedouble;
                program_data.waves[i].frequency = absolute_frequency;
                program_data.waves[i].phase = (float)phase_item->valuedouble;
                program_data.waves[i].offset = (float)offset_item->valuedouble;
            }
            else
                ESP_LOGW(WIFI_SERVER_TAG, "Wave '%d' is ignored due to an invalid frequency of '%.2f' Hz!", i, frequency);
        }
    }

    program_data.number_of_waves = wave_count;

    cJSON_Delete(root);

    return ESP_OK;
}

esp_err_t parse_fft_data(const char* json_data) {
    cJSON* root = cJSON_Parse(json_data);

    // Failed to parse the JSON data:
    if (root == NULL) {
        ESP_LOGE(WIFI_SERVER_TAG, "Failed to parse JSON data!");

        return ESP_FAIL;
    }

    cJSON* window_item = cJSON_GetObjectItem(root, "window");

    // Define a structure to map window names to window configurations:
    typedef struct {
        const char* window_name;
        window_config_t window_config;
    } window_mapping_t;

    // Define the mappings of window names to window configurations:
    const window_mapping_t window_mappings[] = {
        {"HANN_F32", HANN_WINDOW_F32},
        {"BLACKMAN_F32", BLACKMAN_WINDOW_F32},
        {"BLACKMAN_HARRIS_F32", BLACKMAN_HARRIS_WINDOW_F32},
        {"BLACKMAN_NUTTALL_F32", BLACKMAN_NUTTALL_WINDOW_F32},
        {"NUTTALL_F32", NUTTALL_WINDOW_F32},
        {"FLAT_TOP_F32", FLAT_TOP_WINDOW_F32}
    };

    if (cJSON_IsString(window_item)) {
        const char* window_string = window_item->valuestring;
        int num_mappings = sizeof(window_mappings) / sizeof(window_mappings[0]);

        // Iterate through the window mappings and find a match for the provided window name:
        for (int i = 0; i < num_mappings; i++) {
            if (strcmp(window_string, window_mappings[i].window_name) == 0) {
                program_data.window = window_mappings[i].window_config;

                cJSON_Delete(root);

                return ESP_OK;
            }
        }

        ESP_LOGW(WIFI_SERVER_TAG, "Unknown window configuration '%s'!", window_string); // The provided window name does not match any known window configurations.
    } 
    else 
        ESP_LOGW(WIFI_SERVER_TAG, "Invalid window configuration in JSON data!"); // The `window` item is not a string

    cJSON_Delete(root);

    return ESP_OK;
}

esp_err_t parse_dac_data(const char* json_data) {
    cJSON* root = cJSON_Parse(json_data);

    // Failed to parse the JSON data:
    if (root == NULL) {
        ESP_LOGE(WIFI_SERVER_TAG, "Failed to parse the JSON data!");

        return ESP_FAIL;
    }

    cJSON* prevent_overflow_value = cJSON_GetObjectItem(root, "prevent_overflow_value");

    // Check if `prevent_overflow_value` is a boolean true value:
    if (cJSON_IsTrue(prevent_overflow_value)) 
        program_data.prevent_dac_overflow = true;

    // Check if `prevent_overflow_value` is a boolean false value:
    else if (cJSON_IsFalse(prevent_overflow_value))
        program_data.prevent_dac_overflow = false;

    // Failed to parse `prevent_overflow_value` or it is not a boolean:
    else {
        ESP_LOGE(WIFI_SERVER_TAG, "Failed to parse 'prevent_overflow_value', or it is not a boolean!");

        cJSON_Delete(root);

        return ESP_FAIL;
    }

    cJSON_Delete(root);

    return ESP_OK;
}
