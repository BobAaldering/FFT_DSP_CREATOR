#ifndef DAC_COMMUNICATOR_H_
#define DAC_COMMUNICATOR_H_

#include <math.h>

#include "esp_timer.h"
#include "esp_log.h"
#include "driver/dac.h"

#define DAC_COMMUNICATOR_TAG ("DAC_COMMUNICATOR_H_")

#define ESP_VCC_MIN (0.0f)
#define ESP_VCC_MAX (3.3f)

/// @brief Defining a struct called `dac_data`, that contains all the needed data for converting digital samples to analog values over de DAC.
typedef struct dac_data {
    float* digital_samples;   // This field is a pointer to an array of `float` values called `digital_samples`.
    size_t number_of_samples; // This field is a `size_t` variable called `number_of_samples`.

    bool prevent_dac_overflow_conversion; // This field contains a `bool` variable for preventing overflows when converting digital samples to analog values for output over the DAC.

    esp_timer_handle_t timer; // This field is an `esp_timer_handle_t` variable called `timer`.
} dac_data_t;

/// @brief The declaration of an external variable `dac_data`, which means that this variable is defined in another source file (in this case 'main.c').
extern dac_data_t dac_data;

/// @brief This function enables the DAC output and sets up a timer to send a signal over the DAC at a specified sample frequency.
/// @param sample_frequency The frequency at which the signal will be output over the DAC (in Hz).
/// @return An `esp_err_t` type, which is either `ESP_OK` if the function executes successfully or `ESP_FAIL` if there is an error.
extern esp_err_t dac_output_values(size_t sample_frequency);

/// @brief This function handles the DAC timer and outputs a digital value based on the analog value from the DAC data.
/// @param _ The function `dac_timer_handler` takes a `void*` parameter, which is not used in the function. The function uses the following variables:
void dac_timer_handler(void*);

#endif
