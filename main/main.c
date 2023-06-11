#include <stdlib.h>

#include "fft_transform.h"
#include "fir_transform.h"
#include "wave_transform.h"
#include "window_transform.h"

#define N_SAMPLES        (1024) // The number number of samples used by 'FFT'.
#define SIGNAL_FREQUENCY (50)   // The frequency in 'Hz' of the signal produced.
#define SAMPLE_FREQUENCY (200)  // The sample frequency in 'Hz'.

void app_main() {
    size_t n_sample_buffer_length = N_SAMPLES + FIR_DELAY;

    float* samples = calloc(n_sample_buffer_length, sizeof(float));

    fir_data_t data_for_fir = {};
    fft_data_t data_for_fft = {};

    wave_config_t data_for_waves[] = {
        {
            .amplitude = 2.0,
            .frequency = 30.0 / SAMPLE_FREQUENCY,
            .phase = 0.0
        },
        {
            .amplitude = 3.0,
            .frequency = 40.0 / SAMPLE_FREQUENCY,
            .phase = 0.0
        },
        {
            .amplitude = 4.0,
            .frequency = 50.0 / SAMPLE_FREQUENCY,
            .phase = 0.0
        }
    };

    initialize_fir_data(&data_for_fir, FIR_COEFFICIENTS, n_sample_buffer_length);
    initialize_fft_f32(&data_for_fft);

    generate_waves_f32(data_for_waves, samples, n_sample_buffer_length, sizeof(data_for_waves) / sizeof(data_for_waves[0]));

    apply_fft_f32(&data_for_fft, samples, HANN_WINDOW_F32, N_SAMPLES);

    de_initialize_fir_data(&data_for_fir);
    de_initialize_fft_f32(&data_for_fft);

    free(samples);
}
