#ifndef FIR_TRANSFORM_H_
#define FIR_TRANSFORM_H_

#include <stdlib.h>
#include <math.h>

#include "esp_dsp.h"

#include "window_transform.h"

#define FIR_TRANSFORM_TAG  ("FIR_TRANSFORM_H_")

#define DECIMATION            (2)
#define FIR_COEFFICIENTS      (64)
#define FIR_DELAY             (FIR_COEFFICIENTS / DECIMATION)

typedef struct fir_data {
    float* fir_coefficients;
    float* fir_delay_line;

    size_t fir_coefficients_length;

    float* fir_out_data;
} fir_data_t;

extern void initialize_fir_data(fir_data_t* fir_data, size_t fir_coefficients_length, size_t sample_length);
extern void de_initialize_fir_data(fir_data_t* fir_data);

extern void generate_fir_window_coefficients(fir_data_t* fir_data, window_config_t window_config, float transistion_frequency);

extern void apply_fir_filter_f32(fir_data_t* fir_data, window_config_t window_config, float* samples, size_t sample_length);

#endif
