#include "fft_transform.h"

esp_err_t initialize_fft_f32(fft_data_t* fft_data) {
    if (fft_data == NULL) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "fft_data");

        return ESP_OK;
    }

    esp_err_t return_value = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);

    if (return_value != ESP_OK)
        ESP_LOGE(FFT_TRANSFORM_TAG, "It is not possible to initialize the 'FFT'. Error = '%i'!", return_value);

    fft_data->fft_is_initialized = true;

    return ESP_OK;
}

esp_err_t de_initialize_fft_f32(fft_data_t* fft_data) {
    if (fft_data == NULL) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "fft_data");

        return ESP_FAIL;
    }

    dsps_fft2r_deinit_fc32();

    fft_data->fft_is_initialized = false;

    return ESP_OK;
}

esp_err_t apply_fft_f32(fft_data_t* fft_data, float* samples, window_config_t window_config, size_t sample_length) {
    if (fft_data == NULL || samples == NULL) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "The value of '%s' and '%s' could not be 'NULL'!", "fft_data", "samples");

        return ESP_FAIL;
    }

    if (!fft_data->fft_is_initialized) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "The 'FFT' is not initialized yet, call 'initialize_fft_f32' first!");

        return ESP_FAIL;
    }

    float* fft_window = calloc(sample_length, sizeof(float));
    float* fft_y_cf = calloc(sample_length * 2, sizeof(float));

    if (fft_window == NULL || fft_y_cf == NULL) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "One of the allocations failed!");

        return ESP_FAIL;
    }

    float* fft_y_cf_real_part = fft_y_cf;
    float* fft_y_cf_magnitude = &fft_y_cf[sample_length];

    esp_err_t succeeded_window_generation = apply_window_function(fft_window, window_config, sample_length);

    if (succeeded_window_generation != ESP_OK) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "Unknown configuration for the provided window in '%s'!", "window_config");

        return ESP_FAIL;
    }

    for (int i = 0; i < sample_length; i++) {
        fft_y_cf[i * 2 + 0] = samples[i] * fft_window[i];
        fft_y_cf[i * 2 + 1] = 0;
    }

    dsps_fft2r_fc32(fft_y_cf, sample_length);
    dsps_bit_rev_fc32(fft_y_cf, sample_length);
    dsps_cplx2reC_fc32(fft_y_cf, sample_length);

    for (int i = 0; i < sample_length / 2; i++) {
        fft_y_cf_real_part[i] = 10 * log10f((fft_y_cf_real_part[i * 2 + 0] * fft_y_cf_real_part[i * 2 + 0] + fft_y_cf_real_part[i * 2 + 1] * fft_y_cf_real_part[i * 2 + 1]) / sample_length);
        fft_y_cf_magnitude[i] = (fft_y_cf_real_part[i * 2 + 0] * fft_y_cf_real_part[i * 2 + 0] + fft_y_cf_real_part[i * 2 + 1] * fft_y_cf_real_part[i * 2 + 1]) / sample_length;
    }

    ESP_LOGI(FFT_TRANSFORM_TAG, "Signal in log scale:");
    dsps_view(fft_y_cf_real_part, sample_length / 2, 64, 10,  0, 50, '|');

    ESP_LOGI(FFT_TRANSFORM_TAG, "Signal in absolute scale:");
    dsps_view(fft_y_cf_magnitude, sample_length / 2, 64, 10,  0, 2, '|');

    oled_view_fft(fft_y_cf_real_part, sample_length / 2, ssd1306_get_width(&oled_display), ssd1306_get_height(&oled_display), 0, 50);

    free(fft_window);
    free(fft_y_cf);

    return ESP_OK;
}
