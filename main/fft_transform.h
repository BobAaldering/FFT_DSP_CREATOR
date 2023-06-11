#ifndef FFT_TRANSFORM_H_
#define FFT_TRANSFORM_H_

#include <stdlib.h>
#include <math.h>

#include "esp_dsp.h"

#include "window_transform.h"

#define FFT_TRANSFORM_TAG  ("FFT_TRANSFORM_H_")

typedef struct fft_data {
    bool fft_is_initialized;
} fft_data_t;

extern void initialize_fft_f32(fft_data_t* fft_data);
extern void de_initialize_fft_f32(fft_data_t* fft_data);

extern void apply_fft_f32(fft_data_t* fft_data, float* samples, window_config_t window_config, size_t sample_length);

#endif
