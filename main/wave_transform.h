#ifndef WAVE_TRANSFORM_H_
#define WAVE_TRANSFORM_H_

#include <stdlib.h>

#include "esp_dsp.h"

#define WAVE_TRANSFORM_TAG ("WAVE_TRANSFORM_H_")

/// @brief Defining a struct called `wave_config`, that contains all the needed data for creating a custom wave.
typedef struct wave_config {
    float amplitude; // This field contains a `float` for the amplitude of a wave.
    float frequency; // This field contains a `float` for the frequency of a wave.
    float phase;     // This field contains a `float` for the phase of a wave.
    float offset;    // This field contains a `float` for the offset of a wave.
} wave_config_t;

/// @brief This function generates multiple waves with specified configurations and adds them together to create a final waveform.
/// @param wave_configs An array of `wave_config_t` structures that contain the configuration parameters for each wave to be generated.
/// @param samples A pointer to an array of floats where the generated wave samples will be stored.
/// @param sample_length The length of the output sample array.
/// @param number_of_waves The number of waves to generate and add together.
/// @return An `esp_err_t` value, which is either `ESP_OK` if the function executes successfully or `ESP_FAIL` if there is an error.
extern esp_err_t generate_waves_f32(wave_config_t* wave_configs, float* samples, size_t sample_length, size_t number_of_waves);

#endif
