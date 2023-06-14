#include "http_server.h"

void start_wifi_connection(const char* ssid_name, const char* pass_name) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT(); 

    ESP_ERROR_CHECK(esp_wifi_init(&wifi_initiation));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL));

    wifi_config_t wifi_configuration_data = {};

    strcpy((char* )wifi_configuration_data.sta.ssid, ssid_name);
    strcpy((char* )wifi_configuration_data.sta.password, pass_name);
    
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration_data));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}

void start_webserver(httpd_handle_t server_handle) {
    httpd_config_t http_configuration = HTTPD_DEFAULT_CONFIG();

    ESP_ERROR_CHECK(httpd_start(&server_handle, &http_configuration));

    httpd_uri_t wave_uri = {
        .uri = "/wave",
        .method = HTTP_POST,
        .handler = wave_post_handler,
        .user_ctx = NULL
    };

    httpd_uri_t fft_uri = {
        .uri = "/fft",
        .method = HTTP_POST,
        .handler = fft_post_handler,
        .user_ctx = NULL
    };

    httpd_register_uri_handler(server_handle, &wave_uri);
    httpd_register_uri_handler(server_handle, &fft_uri);

    ESP_LOGI(WIFI_SERVER_TAG, "The webserver with all the URI handlers is started!");
}

void stop_webserver(httpd_handle_t server_handle) {
    ESP_ERROR_CHECK(httpd_stop(server_handle));

    ESP_LOGI(WIFI_SERVER_TAG, "The webserver is no longer active!");
}

void wifi_event_handler(void*, esp_event_base_t, int32_t event_id, void*) {
    switch(event_id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(WIFI_SERVER_TAG, "Connecting to your Wi-Fi network...");
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(WIFI_SERVER_TAG, "Connected to your Wi-Fi network!");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGE(WIFI_SERVER_TAG, "Lost your Wi-Fi connection!");
            break;
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(WIFI_SERVER_TAG, "We got an IP-adress!");
            break;
        default:
            ESP_LOGE(WIFI_SERVER_TAG, "No ID of the event found! Please try again...");
            break;
    }
}

esp_err_t wave_post_handler(httpd_req_t* request) {
    char content[MAXIMUM_CONTENT_LENGTH] = {};

    int return_length = httpd_req_recv(request, content, sizeof(content) / sizeof(content[0]));

    if (return_length <= 0) {
        if (return_length == HTTPD_SOCK_ERR_TIMEOUT)
            httpd_resp_send_408(request);

        return ESP_FAIL;
    }

    ESP_LOGI(WIFI_SERVER_TAG, "The 'wave_post_handler' function is invoked, with content '%s'", content);

    ESP_ERROR_CHECK(parse_wave_data(content));

    for (int i = 0; i < NUMBER_OF_SAMPLES; i++)
        program_data.samples[i] = 0;

    ESP_ERROR_CHECK(generate_waves_f32(program_data.waves, program_data.samples, NUMBER_OF_SAMPLES, program_data.number_of_waves));

    const char* response = "Successful execution of the function 'wave_post_handler'!\n";
    httpd_resp_send(request, response, strlen(response));

    return ESP_OK;
}

esp_err_t fft_post_handler(httpd_req_t* request) {
    char content[MAXIMUM_CONTENT_LENGTH] = {};

    int return_length = httpd_req_recv(request, content, sizeof(content) / sizeof(content[0]));

    if (return_length <= 0) {
        if (return_length == HTTPD_SOCK_ERR_TIMEOUT)
            httpd_resp_send_408(request);

        return ESP_FAIL;
    }

    ESP_LOGI(WIFI_SERVER_TAG, "The 'fft_post_handler' function is invoked, with content '%s'", content);

    ESP_ERROR_CHECK(parse_fft_data(content));

    fft_data_t data_for_fft = {};

    ESP_ERROR_CHECK(initialize_fft_f32(&data_for_fft));
    ESP_ERROR_CHECK(apply_fft_f32(&data_for_fft, program_data.samples, program_data.window, NUMBER_OF_SAMPLES));
    ESP_ERROR_CHECK(de_initialize_fft_f32(&data_for_fft));

    const char* response = "Successful execution of the function 'fft_post_handler'!\n";
    httpd_resp_send(request, response, strlen(response));

    return ESP_OK;
}

esp_err_t parse_wave_data(const char* json_data) {
    cJSON* root = cJSON_Parse(json_data);

    if (root == NULL) {
        ESP_LOGE(WIFI_SERVER_TAG, "Failed to parse JSON data!");

        return ESP_FAIL;
    }

    cJSON* sample_frequency_item = cJSON_GetObjectItem(root, "sample_frequency");

    if (cJSON_IsNumber(sample_frequency_item)) 
        program_data.sample_frequency = sample_frequency_item->valueint;

    cJSON* waves_array = cJSON_GetObjectItem(root, "waves");

    if (!cJSON_IsArray(waves_array)) {
        ESP_LOGE(WIFI_SERVER_TAG, "Invalid waves array in JSON data!");

        cJSON_Delete(root);

        return ESP_FAIL;
    }

    int wave_count = cJSON_GetArraySize(waves_array);

    if (wave_count > MAXIMUM_WAVES_LENGTH) {
        ESP_LOGW(WIFI_SERVER_TAG, "Received more waves than supported. Truncating it to '%d' waves!", MAXIMUM_WAVES_LENGTH);

        wave_count = MAXIMUM_WAVES_LENGTH;
    }

    for (int i = 0; i < wave_count; i++) {
        cJSON* wave_item = cJSON_GetArrayItem(waves_array, i);
        cJSON* amplitude_item = cJSON_GetObjectItem(wave_item, "amplitude");
        cJSON* frequency_item = cJSON_GetObjectItem(wave_item, "frequency");
        cJSON* phase_item = cJSON_GetObjectItem(wave_item, "phase");

        if (cJSON_IsNumber(amplitude_item) && cJSON_IsNumber(frequency_item) && cJSON_IsNumber(phase_item)) {
            program_data.waves[i].amplitude = (float)amplitude_item->valuedouble;
            program_data.waves[i].frequency = (float)frequency_item->valuedouble;
            program_data.waves[i].phase = (float)phase_item->valuedouble;
        }
    }

    program_data.number_of_waves = wave_count;

    cJSON_Delete(root);

    return ESP_OK;
}

esp_err_t parse_fft_data(const char* json_data) {
    cJSON* root = cJSON_Parse(json_data);

    if (root == NULL) {
        ESP_LOGE(WIFI_SERVER_TAG, "Failed to parse JSON data!");

        return ESP_FAIL;
    }

    cJSON* window_item = cJSON_GetObjectItem(root, "window");

    typedef struct {
        const char* window_name;
        window_config_t window_config;
    } window_mapping_t;

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

        for (int i = 0; i < num_mappings; i++) {
            if (strcmp(window_string, window_mappings[i].window_name) == 0) {
                program_data.window = window_mappings[i].window_config;

                cJSON_Delete(root);

                return ESP_OK;
            }
        }

        ESP_LOGW(WIFI_SERVER_TAG, "Unknown window configuration '%s'!", window_string);
    } 
    else 
        ESP_LOGW(WIFI_SERVER_TAG, "Invalid window configuration in JSON data!");

    return ESP_OK;
}
