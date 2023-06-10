#include "fft_transform.h"

void initialize_fft_data(fft_data_t* fft_data) {
    if (fft_data == NULL) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "fft_data");

        return;
    }

    float* x_sample_allocated = calloc(N_SAMPLES, sizeof(float));
    float* fft_window_allocated = calloc(N_SAMPLES, sizeof(float));
    float* y_cf_allocated = calloc(N_SAMPLES * 2, sizeof(float));

    if (x_sample_allocated == NULL || fft_window_allocated == NULL || y_cf_allocated == NULL) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "");

        return;
    }

    fft_data->x_sample = x_sample_allocated;
    fft_data->fft_window = fft_window_allocated;
    fft_data->y_cf = y_cf_allocated;

    fft_data->y_cf_real_part = y_cf_allocated;
    fft_data->y_cf_magnitude = &y_cf_allocated[N_SAMPLES];

    fft_data->is_fft_initialized = false;
}

void de_initialize_fft_data(fft_data_t* fft_data) {
    if (fft_data == NULL) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "fft_data");

        return;
    }

    free(fft_data->x_sample);
    free(fft_data->fft_window);
    free(fft_data->y_cf);

    fft_data->y_cf_real_part = NULL;
    fft_data->y_cf_magnitude = NULL;
}

void initialize_fft_f32(fft_data_t* fft_data) {
    if (fft_data == NULL) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "fft_data");

        return;
    }

    esp_err_t return_value = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);

    if (return_value != ESP_OK)
        ESP_LOGE(FFT_TRANSFORM_TAG, "It is not possible to initialize the 'FFT'. Error = '%i'!", return_value);

    fft_data->is_fft_initialized = true;
}

void de_initialize_fft_f32(fft_data_t* fft_data) {
    if (fft_data == NULL) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "fft_data");

        return;
    }

    dsps_fft2r_deinit_fc32();

    fft_data->is_fft_initialized = false;
}

void apply_fft_f32(fft_data_t* fft_data, window_config_t window_config) {
    if (fft_data == NULL) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "fft_data");

        return;
    }

    if (!fft_data->is_fft_initialized) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "The 'FFT' is not initialized yet!");

        return;
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

        selected_window_function(fft_data->fft_window, N_SAMPLES);
    }
    else {
        ESP_LOGE(FFT_TRANSFORM_TAG, "Unknown configuration for the provided window in '%s'!", "window_config");
    
        return;
    }

    for (int i = 0; i < N_SAMPLES; i++) {
        fft_data->y_cf[i * 2 + 0] = fft_data->x_sample[i] * fft_data->fft_window[i];
        fft_data->y_cf[i * 2 + 1] = 0;
    }

    dsps_fft2r_fc32(fft_data->y_cf, N_SAMPLES);
    dsps_bit_rev_fc32(fft_data->y_cf, N_SAMPLES);
    dsps_cplx2reC_fc32(fft_data->y_cf, N_SAMPLES);

    for (int i = 0; i < N_SAMPLES / 2; i++) {
        fft_data->y_cf_real_part[i] = 10 * log10f((fft_data->y_cf_real_part[i * 2 + 0] * fft_data->y_cf_real_part[i * 2 + 0] + fft_data->y_cf_real_part[i * 2 + 1] * fft_data->y_cf_real_part[i * 2 + 1]) / N_SAMPLES);
        fft_data->y_cf_magnitude[i] = (fft_data->y_cf_real_part[i * 2 + 0] * fft_data->y_cf_real_part[i * 2 + 0] + fft_data->y_cf_real_part[i * 2 + 1] * fft_data->y_cf_real_part[i * 2 + 1]) / N_SAMPLES;
    }

    ESP_LOGI(FFT_TRANSFORM_TAG, "Signal in log scale:");
    dsps_view(fft_data->y_cf_real_part, N_SAMPLES / 2, 64, 10,  -60, 40, '|');

    ESP_LOGI(FFT_TRANSFORM_TAG, "Signal in absolute scale:");
    dsps_view(fft_data->y_cf_magnitude, N_SAMPLES / 2, 64, 10,  0, 2, '|');
}

void generate_waves_f32(wave_config_t* wave_configs, fft_data_t* fft_data, size_t number_of_waves) {
    if (wave_configs == NULL || fft_data == NULL) {
        ESP_LOGE(FFT_TRANSFORM_TAG, "The values of '%s' and '%s' could not be 'NULL'!", "wave_configs", "fft_data");

        return;
    }

    for (int i = 0; i < number_of_waves; i++) {
        float current_amplitude = wave_configs[i].amplitude;
        float current_frequency = wave_configs[i].frequency;
        float current_phase = wave_configs[i].phase;

        float* current_wave = calloc(N_SAMPLES, sizeof(float));

        if (current_wave == NULL)
            ESP_LOGE(FFT_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "current_wave");

        dsps_tone_gen_f32(current_wave, N_SAMPLES, current_amplitude, current_frequency, current_phase);

        for (int j = 0; j < N_SAMPLES; j++)
            fft_data->x_sample[j] += current_wave[j];

        free(current_wave);
    }
}
