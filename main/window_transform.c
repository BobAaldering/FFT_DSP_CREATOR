#include "window_transform.h"

esp_err_t apply_window_function(float* window, window_config_t window_config, size_t window_length) {
    if (window == NULL) {
        ESP_LOGE(WINDOW_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "window");

        return ESP_FAIL;
    }

    window_function window_config_lookup[] = {
        dsps_wind_hann_f32,
        dsps_wind_blackman_f32,
        dsps_wind_blackman_harris_f32,
        dsps_wind_blackman_nuttall_f32,
        dsps_wind_nuttall_f32,
        dsps_wind_flat_top_f32
    };

    if (window_config >= 0 && window_config < sizeof(window_config_lookup) / sizeof(window_config_lookup[0])) {
        window_function selected_window_function = window_config_lookup[window_config];
        
        selected_window_function(window, window_length);

        return ESP_OK;
    }

    return ESP_FAIL;
}
