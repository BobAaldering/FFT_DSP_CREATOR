#include "display_communicator.h"

void initialize_oled(size_t screen_width, size_t screen_height) {
    i2c_master_init(&oled_display, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
    ssd1306_init(&oled_display, screen_width, screen_height);

    ssd1306_clear_screen(&oled_display, false);
    ssd1306_contrast(&oled_display, 0xFF);
}

void oled_view_fft(float* data, int32_t length, int width, int height, float minimum, float maximum) {
    uint8_t* view_data = calloc(width * height, sizeof(uint8_t));
    float* view_data_minimum = calloc(width, sizeof(float));
    float* view_data_maximum = calloc(width, sizeof(float));

    for (int i = 0; i < width; i++) {
        view_data_minimum[i] = maximum;
        view_data_maximum[i] = minimum;
    }

    float x_step = (float)width / (float)length;
    float y_step = (float)(height - 1) / (maximum - minimum);

    float data_minimum = FLT_MAX;
    float data_maximum = -FLT_MAX;

    for (int i = 0; i < length; i++) {
        int x_position = i * x_step;
        float data_i = data[i];

        if (data_i < view_data_minimum[x_position]) 
            view_data_minimum[x_position] = data_i;
        
        if (data_i > view_data_maximum[x_position]) 
            view_data_maximum[x_position] = data_i;

        if (view_data_minimum[x_position] < minimum) 
            view_data_minimum[x_position] = minimum;
        
        if (view_data_maximum[x_position] > maximum) 
            view_data_maximum[x_position] = maximum;

        if (data_i > data_maximum) 
            data_maximum = data_i;

        if (data_i < data_minimum) 
            data_minimum = data_i;
    }

    for (int x = 0; x < width; x++) {
        int y_count = (view_data_maximum[x] - view_data_minimum[x]) * y_step + 1;
        int y_start = (maximum - view_data_maximum[x]) * y_step;

        for (int y = 0; y < y_count; y++) {
            int y_pos = y_start + y;

            view_data[y_pos * width + x] = 1;
        }
    }

    ssd1306_clear_screen(&oled_display, false);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            int value = !view_data[index];
            
            _ssd1306_pixel(&oled_display, x, y, value);
        }
    }

    free(view_data);
    free(view_data_minimum);
    free(view_data_maximum);
}