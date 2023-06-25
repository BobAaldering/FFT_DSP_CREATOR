#include "wave_transform.h"

esp_err_t generate_waves_f32(wave_config_t* wave_configs, float* samples, size_t sample_length, size_t number_of_waves) {
    // Check if `wave_configs` and samples pointers are valid:
    if (wave_configs == NULL || samples == NULL) {
        ESP_LOGE(WAVE_TRANSFORM_TAG, "The values of '%s' and '%s' could not be 'NULL'!", "wave_configs", "samples");

        return ESP_FAIL;
    }

    // Generate waves based on the provided wave configurations:
    for (int i = 0; i < number_of_waves; i++) {
        float current_amplitude = wave_configs[i].amplitude;
        float current_frequency = wave_configs[i].frequency;
        float current_phase = wave_configs[i].phase;
        float current_offset = wave_configs[i].offset;

        float* current_wave = calloc(sample_length, sizeof(float)); // Allocate memory for the current wave.

        // Check if the memory allocation was successful:
        if (current_wave == NULL)
            ESP_LOGE(WAVE_TRANSFORM_TAG, "The value of '%s' could not be 'NULL'!", "current_wave");

        dsps_tone_gen_f32(current_wave, sample_length, current_amplitude, current_frequency, current_phase); // Generate the current wave using the tone generator function.

        // Add the current wave to the samples array, taking into account the offset:
        for (int j = 0; j < sample_length; j++)
            samples[j] += current_wave[j] + current_offset;

        free(current_wave); // Free the memory allocated for the current wave.
    }

    return ESP_OK;
}
