#include "wave_transform.h"

esp_err_t generate_waves_f32(wave_config_t* wave_configs, float* samples, size_t sample_length, size_t number_of_waves) {
    if (wave_configs == NULL || samples == NULL) {
        ESP_LOGE(WAVE_TRANSFORM_TAG, "The values of '%s' and '%s' could not be 'NULL'!", "wave_configs", "samples");

        return ESP_FAIL;
    }

    for (int i = 0; i < number_of_waves; i++) {
        float current_amplitude = wave_configs[i].amplitude;
        float current_frequency = wave_configs[i].frequency;
        float current_phase = wave_configs[i].phase;

        float* current_wave = calloc(sample_length, sizeof(float));

        if (current_wave == NULL)
            ESP_LOGE(WAVE_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "current_wave");

        dsps_tone_gen_f32(current_wave, sample_length, current_amplitude, current_frequency, current_phase);

        for (int j = 0; j < sample_length; j++)
            samples[j] += current_wave[j];

        free(current_wave);
    }

    return ESP_OK;
}
