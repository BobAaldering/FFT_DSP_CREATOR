#ifndef FFT_TRANSFORM_H_
#define FFT_TRANSFORM_H_

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "esp_dsp.h"

#include "display_communicator.h"
#include "window_transform.h"

#define FFT_TRANSFORM_TAG ("FFT_TRANSFORM_H_")

typedef struct fft_data {
    bool fft_is_initialized;
} fft_data_t;

extern SSD1306_t oled_display;

extern esp_err_t initialize_fft_f32(fft_data_t* fft_data);
extern esp_err_t de_initialize_fft_f32(fft_data_t* fft_data);

extern esp_err_t apply_fft_f32(fft_data_t* fft_data, float* samples, window_config_t window_config, size_t sample_length);

#endif
