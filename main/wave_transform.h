#ifndef WAVE_TRANSFORM_H_
#define WAVE_TRANSFORM_H_

#include <stdlib.h>

#include "esp_dsp.h"

#define WAVE_TRANSFORM_TAG ("WAVE_TRANSFORM_H_")

typedef struct wave_config {
    float amplitude;
    float frequency;
    float phase;
} wave_config_t;

extern void generate_waves_f32(wave_config_t* wave_configs, float* samples, size_t sample_length, size_t number_of_waves);

#endif
