#ifndef DAC_COMMUNICATOR_H_
#define DAC_COMMUNICATOR_H_

#include <string.h>
#include <math.h>

#include "esp_timer.h"
#include "driver/dac.h"

typedef struct dac_data {
    float* digital_samples;
    size_t number_of_samples;
    size_t sent_values;

    esp_timer_handle_t timer;
} dac_data_t;

extern dac_data_t dac_data;

extern esp_err_t dac_output_values();

void timer_handler(void*);

#endif
