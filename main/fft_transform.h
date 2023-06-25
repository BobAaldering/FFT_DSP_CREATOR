#ifndef FFT_TRANSFORM_H_
#define FFT_TRANSFORM_H_

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "esp_dsp.h"

#include "display_communicator.h"
#include "window_transform.h"

#define FFT_TRANSFORM_TAG ("FFT_TRANSFORM_H_")

/// @brief Defining a struct called `fft_data`, that contains a boolean indicating whether the FFT is initialized.
typedef struct fft_data {
    bool fft_is_initialized; // This field contains a `bool`, indicating if the FFT is successfully initialized.
} fft_data_t;

/// @brief This function initializes the FFT with a given maximum size and sets a flag indicating that the FFT is initialized.
/// @param fft_data A pointer to a struct that contains data related to the FFT operation.
/// @return An `esp_err_t` value, which is either `ESP_OK` if the initialization was successful or an error code if it failed.
extern esp_err_t initialize_fft_f32(fft_data_t* fft_data);

/// @brief This function de-initializes a FFT data structure.
/// @param fft_data A pointer to a struct that contains data related to the FFT (Fast Fourier Transform) operation.
/// @return An `esp_err_t` value, which is either `ESP_OK` if the initialization was successful or an error code if it failed.
extern esp_err_t de_initialize_fft_f32(fft_data_t* fft_data);

/// @brief This function applies a FFT to a set of float samples, using a provided window configuration, and outputs the results in both log and absolute scales.
/// @param fft_data A pointer to the FFT data structure that holds the necessary information for the FFT transformation.
/// @param samples An array of float values representing the audio samples to be transformed.
/// @param window_config An enumeration type that contains the configuration parameters for the window function to be applied to the input signal before performing the FFT.
/// @param sample_length The length of the input signal in samples.
/// @param sample_frequency The frequency at which the signal is sampled, measured in Hz (Hertz).
/// @return An `esp_err_t` value, which is either `ESP_OK` if the initialization was successful or an error code if it failed.
extern esp_err_t apply_fft_f32(fft_data_t* fft_data, float* samples, window_config_t window_config, size_t sample_length, size_t sample_frequency);

#endif
