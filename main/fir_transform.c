#include "fir_transform.h"

void initialize_fir_data(fir_data_t* fir_data, size_t fir_coefficients_length, size_t sample_length) {
    if (fir_data == NULL) {
        ESP_LOGE(FIR_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "fir_data");

        return;
    }

    float* fir_coefficients_allocated = calloc(fir_coefficients_length, sizeof(float));
    float* fir_delay_line_allocated = calloc(fir_coefficients_length, sizeof(float));
    float* fir_out_data_allocated = calloc(sample_length, sizeof(float));

    if (fir_coefficients_allocated == NULL || fir_delay_line_allocated == NULL || fir_out_data_allocated == NULL) {
        ESP_LOGE(FIR_TRANSFORM_TAG, "One of the allocations for the 'fft_data' structure failed!");

        return;
    }

    fir_data->fir_coefficients = fir_coefficients_allocated;
    fir_data->fir_delay_line = fir_delay_line_allocated;

    fir_data->fir_coefficients_length = fir_coefficients_length;

    fir_data->fir_out_data = fir_out_data_allocated;
}

void de_initialize_fir_data(fir_data_t* fir_data) {
    if (fir_data == NULL) {
        ESP_LOGE(FIR_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "fir_data");

        return;
    }

    free(fir_data->fir_coefficients);
    free(fir_data->fir_delay_line);
    free(fir_data->fir_out_data);

    fir_data->fir_coefficients_length = 0;
}

void generate_fir_window_coefficients(fir_data_t* fir_data, window_config_t window_config, float transistion_frequency) {
    bool is_odd_window_length = (fir_data->fir_coefficients_length % 2) ? true : false;
    float fir_order = (float)(fir_data->fir_coefficients_length - 1);

    float* current_fir_window = calloc(fir_data->fir_coefficients_length, sizeof(float));

    if (current_fir_window == NULL)
        ESP_LOGE(FIR_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "current_fir_window");

    bool succeeded_window_generation = apply_window_function(current_fir_window, window_config, fir_data->fir_coefficients_length);

    if (!succeeded_window_generation) {
        ESP_LOGE(FIR_TRANSFORM_TAG, "Unknown configuration for the provided window in '%s'!", "window_config");

        free(current_fir_window);

        return;
    }

    for (int i = 0; i < fir_data->fir_coefficients_length; i++) {
        if ((i == fir_order / 2) && is_odd_window_length)
            fir_data->fir_coefficients[i] = 2 * transistion_frequency;
        else
            fir_data->fir_coefficients[i] = sinf(2 * M_PI * transistion_frequency * (i - fir_order / 2));

        fir_data->fir_coefficients[i] *= current_fir_window[i];
    }

    free(current_fir_window);
}

void apply_fir_filter_f32(fir_data_t* fir_data, window_config_t window_config, float* samples, size_t sample_length) {
    fir_f32_t fir_filter;

    float transistion_frequency = 0.5 / (float)DECIMATION;

    generate_fir_window_coefficients(fir_data, window_config, transistion_frequency);
    dsps_fird_init_f32(&fir_filter, fir_data->fir_coefficients, fir_data->fir_delay_line, fir_data->fir_coefficients_length, DECIMATION, 0);

    dsps_fird_f32_ansi(&fir_filter, samples, fir_data->fir_out_data, sample_length);
}
