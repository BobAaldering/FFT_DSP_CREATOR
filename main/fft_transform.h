#ifndef FFT_TRANSFORM_H_
#define FFT_TRANSFORM_H_

#include <stdlib.h>
#include <math.h>

#include "esp_dsp.h"

#define FFT_TRANSFORM_TAG  ("FFT_TRANSFORM_H_")

#define N_SAMPLES        (1024) // The number number of samples used by 'FFT'.
#define SIGNAL_FREQUENCY (50)   // The frequency in 'Hz' of the signal produced.
#define SAMPLE_FREQUENCY (200)  // The sample frequency in 'Hz'.

/// @brief This is a function pointer type for window functions.
typedef void (*window_function)(float* window, int length);

/// @brief This is an enumeration for window configuration options.
typedef enum window_config {
    HANN_WINDOW_F32,             // Usage of the 'Hanning' window.
    BLACKMAN_WINDOW_F32,         // Usage of the 'Blackman' window.
    BLACKMAN_HARRIS_WINDOW_F32,  // Usage of the 'Blackman-Harris' window.
    BLACKMAN_NUTTALL_WINDOW_F32, // Usage of the 'Blackman-Nuttall' window.
    NUTTALL_WINDOW_F32,          // Usage of the 'Nutall' window.
    FLAT_TOP_WINDOW_F32          // Usage of the 'Flat-top' window.
} window_config_t;

/// @brief This is a structure for wave configuration parameters.
typedef struct wave_config {
    float amplitude; // Field for the amplitude of the wave.
    float frequency; // Field for the frequency of the wave.
    float phase;     // Field for the phase of the wave.
} wave_config_t;

/// @brief This is a structure for the 'FFT' data.
typedef struct fft_data {
    float* x_sample;         // Field to a block of memory with the input samples.

    float* fft_window;       // Field to a block of memory with the window coefficients for the 'FFT'.
    float* y_cf;             // Field to a block of memory for the complex output of the 'FFT'.

    float* y_cf_real_part;            // Field that points to the real part of the 'FFT' output.
    float* y_cf_magnitude;            // Field that points to the absolute values of the 'FFT' output.

    bool is_fft_initialized; // Flag indicating wheter the 'FFT' is initialized.
} fft_data_t;

/// @brief This function initializes the 'FFT' data structure.
/// @param fft_data A pointer to the fft_data_t structure to be initialized.
extern void initialize_fft_data(fft_data_t* fft_data);

/// @brief This function deinitializes the 'FFT' data structure and frees allocated memory.
/// @param fft_data A pointer to the fft_data_t structure to be deinitialized.
extern void de_initialize_fft_data(fft_data_t* fft_data);

/// @brief This function initializes the 'FFT' library with 32-bit floating-point precision.
/// @param fft_data  A pointer to the 'fft_data_t' structure for which the 'FFT' should be initialized.
extern void initialize_fft_f32(fft_data_t* fft_data);

/// @brief This function deinitializes the 'FFT' library with 32-bit floating-point precision.
/// @param fft_data A pointer to the 'fft_data_t' structure for which the 'FFT' should be deinitialized.
extern void de_initialize_fft_f32(fft_data_t* fft_data);

/// @brief This function applies the 'FFT' to the given data with the specified window configuration.
/// @param fft_data A pointer to the 'fft_data_t' structure on which the 'FFT' should be applied.
/// @param window_config 
extern void apply_fft_f32(fft_data_t* fft_data, window_config_t window_config);

/// @brief This function generates waveforms based on the given configurations and adds them to the 'x_sample' array.
/// @param wave_configs An array of 'wave_config_t' structure objects containing the wave configurations.
/// @param fft_data A pointer to the 'fft_data_t' structure where the generated waveforms are added.
/// @param number_of_waves The number of waveforms to be generated.
extern void generate_waves_f32(wave_config_t* wave_configs, fft_data_t* fft_data, size_t number_of_waves);

#endif
