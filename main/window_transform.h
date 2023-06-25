#ifndef WINDOW_TRANSFORM_H_
#define WINDOW_TRANSFORM_H_

#include <stdlib.h>

#include "esp_dsp.h"

#define WINDOW_TRANSFORM_TAG ("WINDOW_TRANSFORM_H_")

/// @brief This is a function pointer, that takes a pointer to a window together with the length.
typedef void (*window_function)(float* window, int length);

/// @brief This is an enumeration called `window_config_t` with several values representing different types of window functions. 
typedef enum window_config {
    HANN_WINDOW_F32,
    BLACKMAN_WINDOW_F32,
    BLACKMAN_HARRIS_WINDOW_F32,
    BLACKMAN_NUTTALL_WINDOW_F32,
    NUTTALL_WINDOW_F32,
    FLAT_TOP_WINDOW_F32
} window_config_t;

/// @brief This function applies a selected window function to a given window array.
/// @param window A pointer to an array of floats that represents the window function to be applied.
/// @param window_config An enum value representing the type of window function to be applied. The possible values are defined in the `window_config_t` enum.
/// @param window_length The length of the window function to be applied to the input signal.
/// @return An `esp_err_t` type, which is either `ESP_OK` or `ESP_FAIL`.
extern esp_err_t apply_window_function(float* window, window_config_t window_config, size_t window_length);

#endif
