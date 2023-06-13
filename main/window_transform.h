#ifndef WINDOW_TRANSFORM_H_
#define WINDOW_TRANSFORM_H_

#include <stdlib.h>

#include "esp_dsp.h"

#define WINDOW_TRANSFORM_TAG ("WINDOW_TRANSFORM_H_")

typedef void (*window_function)(float* window, int length);

typedef enum window_config {
    HANN_WINDOW_F32,
    BLACKMAN_WINDOW_F32,
    BLACKMAN_HARRIS_WINDOW_F32,
    BLACKMAN_NUTTALL_WINDOW_F32,
    NUTTALL_WINDOW_F32,
    FLAT_TOP_WINDOW_F32
} window_config_t;

extern bool apply_window_function(float* window, window_config_t window_config, size_t window_length);

#endif
