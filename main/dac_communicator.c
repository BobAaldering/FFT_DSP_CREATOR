#include "dac_communicator.h"

esp_err_t dac_output_values(size_t sample_frequency) {
    // Check if the sample frequency is valid:
    if (sample_frequency == 0) {
        ESP_LOGE(DAC_COMMUNICATOR_TAG, "The sample frequency cannot be equal to zero, because then no signal can be output over the DAC!");

        return ESP_FAIL;
    }

    ESP_ERROR_CHECK(dac_output_enable(DAC_CHANNEL_1)); // Enable the output for the DAC.

    // Stop and reinitialize the current DAC timer if it exists:
    if (dac_data.timer != NULL) {
        ESP_LOGI(DAC_COMMUNICATOR_TAG, "The current timer for sending the signal over the DAC will be stopped, and reinitialized!");

        ESP_ERROR_CHECK(esp_timer_stop(dac_data.timer));
        ESP_ERROR_CHECK(esp_timer_delete(dac_data.timer));
    }
    
    // Create the timer for DAC output:
    esp_timer_create_args_t timer_arguments = {
        .callback = &dac_timer_handler,
        .name = "dac_timer"
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_arguments, &dac_data.timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(dac_data.timer, (1000 / sample_frequency) * 1000));

    return ESP_OK;
}

void dac_timer_handler(void*) {
    static size_t current_index = 0;

    float analog_value = dac_data.digital_samples[current_index]; // Retrieve the current value of a digital sample.

    // Apply DAC overflow prevention (if it is enabled):
    if (dac_data.prevent_dac_overflow_conversion) 
        analog_value = fmaxf(ESP_VCC_MIN, fminf(analog_value, ESP_VCC_MAX)); 

    uint8_t digital_value = (analog_value * 255) / ESP_VCC_MAX;        // Convert the analog value to a digital value.

    ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_1, digital_value)); // Output the digital value to the DAC.

    current_index = (current_index + 1) % dac_data.number_of_samples;  // Update the current index to the next sample.
}
