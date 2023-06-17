#include "dac_communicator.h"

esp_err_t dac_output_values() {
    ESP_ERROR_CHECK(dac_output_enable(DAC_CHANNEL_1));
    
    const esp_timer_create_args_t timer_arguments = {
        .callback = &timer_handler,
        .name = "dac_timer"
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_arguments, &dac_data.timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(dac_data.timer, (1000 / 200) * 1000));

    return ESP_OK;
}

void timer_handler(void*) {
    static size_t current_index = 0;

    int rounded_value = round(dac_data.digital_samples[current_index]);

    printf("Run: %d, with value: %d\n", dac_data.sent_values, (uint8_t)(rounded_value));

    ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_1, 150));

    current_index = (current_index + 1) % dac_data.number_of_samples;

    dac_data.sent_values++;

    if (dac_data.sent_values >= dac_data.number_of_samples) {
        ESP_ERROR_CHECK(esp_timer_stop(dac_data.timer));
        ESP_ERROR_CHECK(esp_timer_delete(dac_data.timer));
    }
}
