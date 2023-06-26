#include "display_communicator.h"

void initialize_oled(size_t screen_width, size_t screen_height) {
    i2c_master_init(&oled_display, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO); // Initialize the 'I2C' master with the specified 'GPIO' pins.
    ssd1306_init(&oled_display, screen_width, screen_height);                            // Initialize the SSD1306 OLED display with the specified screen dimensions.

    ssd1306_clear_screen(&oled_display, false); // Clear the screen of the OLED display.
    ssd1306_contrast(&oled_display, 0xFF);      // Set the contrast of the OLED display to the maximum value (that is '0xFF').
}

esp_err_t oled_view_startup(char* header_line, char* version_line) {
    // Check if the length of either line exceeds the maximum allowed length:
    if (strlen(header_line) > MAXIMUM_LINE_LENGTH || strlen(version_line) > MAXIMUM_LINE_LENGTH) {
        ESP_LOGE(DISPLAY_COMMUNICATOR_TAG, "One of the lines exceeds the maximum length!");

        return ESP_FAIL;
    }
    
    ssd1306_clear_screen(&oled_display, false); // Clear the screen of the OLED display.

    ssd1306_display_text(&oled_display, 3, header_line, strlen(header_line), false);   // Display the header line at the specified position (line three) on the OLED display.
    ssd1306_display_text(&oled_display, 7, version_line, strlen(version_line), false); // Display the version line at the specified position (line seven) on the OLED display.

    return ESP_OK;
}

esp_err_t oled_view_info(char* message_line) {
    // Check if the length of the message line exceeds the maximum allowed length:
    if (strlen(message_line) > MAXIMUM_LINE_LENGTH) {
        ESP_LOGE(DISPLAY_COMMUNICATOR_TAG, "The 'message_line' exceeds the maximum length!");

        return ESP_FAIL;
    }

    ssd1306_clear_screen(&oled_display, false); // Clear the screen of the OLED display.

    char* info_message_header = " INFO"; // Define the header for the info message.

    ssd1306_display_text_x3(&oled_display, 0, info_message_header, strlen(info_message_header), false);                                                      // Display the info message header on the OLED display using a large font size.
    ssd1306_line(&oled_display, 0, ssd1306_get_height(&oled_display) - 35, ssd1306_get_width(&oled_display), ssd1306_get_height(&oled_display) - 35, false); // Draw a horizontal line below the info message header.
    ssd1306_display_text_cursor(&oled_display, ssd1306_get_height(&oled_display) - 15, 0, message_line, strlen(message_line), false);                        // Display the message line at the bottom of the OLED display.

    return ESP_OK;
}

esp_err_t oled_view_error(char* message_line) {
    // Check if the length of the message line exceeds the maximum allowed length:
    if (strlen(message_line) > MAXIMUM_LINE_LENGTH) {
        ESP_LOGE(DISPLAY_COMMUNICATOR_TAG, "The 'message_line' exceeds the maximum length!");

        return ESP_FAIL;
    }

    ssd1306_clear_screen(&oled_display, false); // Clear the screen of the OLED display.

    char* error_message_header = "ERROR"; // Define the header for the error message.

    ssd1306_display_text_x3(&oled_display, 0, error_message_header, strlen(error_message_header), false);                                                    // Display the error message header on the OLED display using a large font size.
    ssd1306_line(&oled_display, 0, ssd1306_get_height(&oled_display) - 35, ssd1306_get_width(&oled_display), ssd1306_get_height(&oled_display) - 35, false); // Draw a horizontal line below the error message header.
    ssd1306_display_text_cursor(&oled_display, ssd1306_get_height(&oled_display) - 15, 0, message_line, strlen(message_line), false);                        // Display the message line at the bottom of the OLED display.

    return ESP_OK;
}

esp_err_t oled_view_fft(float* fft_data, uint32_t fft_data_length, uint32_t sample_data_length, size_t sample_frequency, float y_min_magnitude_scale, float y_max_magnitude_scale) {
    // Check if the sample frequency exceeds the maximum allowed value:
    if (sample_frequency > MAXIMUM_SAMPLE_FREQUENCY) {
        ESP_LOGE(DISPLAY_COMMUNICATOR_TAG, "The sample frequency (with value '%d' Hz) could not be greater than '%d' Hz due to limited pixels on the OLED!", sample_frequency, MAXIMUM_SAMPLE_FREQUENCY);

        return ESP_FAIL;
    }
    
    // Get the screen width and height of the OLED display:
    int screen_width = ssd1306_get_width(&oled_display);
    int screen_height = ssd1306_get_height(&oled_display);

    // Calculate the actual width and height available for displaying the FFT data:
    int actual_fft_width = screen_width - FFT_SCREEN_WIDTH;
    int actual_fft_height = screen_height - FFT_SCREEN_HEIGHT;

    // Calculate the step size for mapping x and y coordinates to the actual FFT data:
    float x_step = (float)actual_fft_width / (float)fft_data_length;
    float y_step = (float)(actual_fft_height - 1) / (y_max_magnitude_scale - y_min_magnitude_scale);

    ssd1306_clear_screen(&oled_display, false); // Clear the screen of the OLED display.

    // Calculate the x-coordinate values for displaying frequency information:
    float x_value_mid = fft_data_length / 2.0f;
    float x_value_max = fft_data_length;

    int x_mid = (int)(x_value_mid * x_step);
    int x_max = (int)(x_value_max * x_step);

    // Convert the x-coordinate values to frequency scales for display:
    char x_mid_buffer[UNIT_BUFFER_LENGTH] = {};
    char x_max_buffer[UNIT_BUFFER_LENGTH] = {};

    int x_max_frequency_scale = ((sample_data_length * sample_frequency) / sample_data_length) / 2;

    itoa(x_max_frequency_scale / 2, x_mid_buffer, 10);
    itoa(x_max_frequency_scale, x_max_buffer, 10);

    size_t x_mid_buffer_length = strlen(x_mid_buffer);
    size_t x_max_buffer_length = strlen(x_max_buffer);

    // Display the x-coordinate frequency scales on the OLED display:
    ssd1306_display_text_cursor(&oled_display, 63, x_mid - (x_mid_buffer_length * 2.5), x_mid_buffer, x_mid_buffer_length, false);
    ssd1306_display_text_cursor(&oled_display, 63, x_max - (x_max_buffer_length * 2.5), x_max_buffer, x_max_buffer_length, false);

    // Calculate the y-coordinate values for displaying magnitude information:
    float y_value_min = y_min_magnitude_scale;
    float y_value_mid = (y_min_magnitude_scale + y_max_magnitude_scale) / 2.0f;
    float y_value_max = y_max_magnitude_scale;

    int y_min = (int)((y_max_magnitude_scale - y_value_min) * y_step);
    int y_mid = (int)((y_max_magnitude_scale - y_value_mid) * y_step);
    int y_max = (int)((y_max_magnitude_scale - y_value_max) * y_step);

    // Convert the y-coordinate values to magnitude scales for display:
    char y_min_buffer[UNIT_BUFFER_LENGTH] = {};
    char y_mid_buffer[UNIT_BUFFER_LENGTH] = {};
    char y_max_buffer[UNIT_BUFFER_LENGTH] = {};

    itoa(y_min_magnitude_scale, y_min_buffer, 10);
    itoa(y_max_magnitude_scale / 2, y_mid_buffer, 10);
    itoa(y_max_magnitude_scale, y_max_buffer, 10);

    size_t y_min_buffer_length = strlen(y_min_buffer);
    size_t y_mid_buffer_length = strlen(y_mid_buffer);
    size_t y_max_buffer_length = strlen(y_max_buffer);

    // Display the y-coordinate magnitude scales on the OLED display:
    ssd1306_display_text_cursor(&oled_display, y_min, 0, y_min_buffer, y_min_buffer_length, false);
    ssd1306_display_text_cursor(&oled_display, y_mid, 0, y_mid_buffer, y_mid_buffer_length, false);
    ssd1306_display_text_cursor(&oled_display, y_max, 0, y_max_buffer, y_max_buffer_length, false);
    
    // Allocate memory for `view_data`:
    uint8_t* view_data = calloc(screen_width * screen_height, sizeof(uint8_t));
    float* view_data_minimum = calloc(screen_width, sizeof(float));
    float* view_data_maximum = calloc(screen_width, sizeof(float));

    // Initialize the `view_data` arrays with minimum and maximum values:
    for (int i = 0; i < screen_width; i++) {
        view_data_minimum[i] = y_max_magnitude_scale;
        view_data_maximum[i] = y_min_magnitude_scale;
    }

    // Initialize the minimum and maximum data values:
    float data_minimum = FLT_MAX;
    float data_maximum = -FLT_MAX;

    // Update the `view_data` arrays and find the overall minimum and maximum data values:
    for (int i = 0; i < fft_data_length; i++) {
        int x_position = i * x_step;
        float data_i = fft_data[i];

        // Update the minimum and maximum values for each x position:
        if (data_i < view_data_minimum[x_position])
            view_data_minimum[x_position] = data_i;

        if (data_i > view_data_maximum[x_position])
            view_data_maximum[x_position] = data_i;

        // Clamp the values within the specified y magnitude scale range:
        if (view_data_minimum[x_position] < y_min_magnitude_scale)
            view_data_minimum[x_position] = y_min_magnitude_scale;

        if (view_data_maximum[x_position] > y_max_magnitude_scale)
            view_data_maximum[x_position] = y_max_magnitude_scale;

        // Update the overall minimum and maximum values:
        if (data_i > data_maximum)
            data_maximum = data_i;

        if (data_i < data_minimum)
            data_minimum = data_i;
    }

    // Convert the `view_data` arrays into a binary representation for displaying:
    for (int x = 0; x < actual_fft_width; x++) {
        int y_count = (view_data_maximum[x] - view_data_minimum[x]) * y_step + 1;
        int y_start = (y_max_magnitude_scale - view_data_maximum[x]) * y_step;

        for (int y = 0; y < y_count; y++) {
            int y_pos = y_start + y;

            view_data[y_pos * actual_fft_width + x] = 1;
        }
    }

    // Display the `view_data` on the OLED display:
    for (int y = 0; y < actual_fft_height; y++) {
        for (int x = 0; x < actual_fft_width; x++) {
            int index = y * actual_fft_width + x;
            int value = !view_data[index];

            ssd1306_pixel(&oled_display, x + 20, y, value);
        }
    }

    // Free the allocated memory for the `view_data`:
    free(view_data);
    free(view_data_minimum);
    free(view_data_maximum);

    return ESP_OK;
}
