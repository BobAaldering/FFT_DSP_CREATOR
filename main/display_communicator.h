#ifndef DISPLAY_COMMUNICATOR_H_
#define DISPLAY_COMMUNICATOR_H_

#include <float.h>
#include <string.h>

#include "esp_log.h"

#include "ssd1306.h"

#define DISPLAY_COMMUNICATOR_TAG ("DISPLAY_COMMUNICATOR_H_")

#define MAXIMUM_LINE_LENGTH (15)
#define UNIT_BUFFER_LENGTH (5)

#define FFT_SCREEN_WIDTH (20)
#define FFT_SCREEN_HEIGHT (20)

#define MAXIMUM_SAMPLE_FREQUENCY (1000)

/// @brief The declaration of an external variable `oled_display`, which means that this variable is defined in another source file (in this case 'main.c').
extern SSD1306_t oled_display;

/// @brief This function initializes an OLED display with a given screen width and height.
/// @param screen_width The width of the OLED display screen in pixels.
/// @param screen_height The height of the OLED display screen in pixels.
extern void initialize_oled(size_t screen_width, size_t screen_height);

/// @brief This function displays two lines of text on an OLED display and returns an error if either line exceeds the maximum length.
/// @param header_line A string containing the header text to be displayed on the OLED screen.
/// @param version_line The version number or version information that will be displayed on the OLED screen
/// @return An `esp_err_t` type, which is either `ESP_OK` if the function executes successfully or `ESP_FAIL` if there is an error.
extern esp_err_t oled_view_startup(char* header_line, char* version_line);

/// @brief This function displays an information message on an OLED display.
/// @param message_line A pointer to a character array containing the message to be displayed on the OLED screen.
/// @return An `esp_err_t` type, which is either `ESP_OK` if the function executes successfully or `ESP_FAIL` if there is an error.
extern esp_err_t oled_view_info(char* message_line);

/// @brief This function displays an error message on an OLED display.
/// @param message_line A pointer to a character array containing the error message to be displayed on the OLED screen.
/// @return An `esp_err_t` type, which is either `ESP_OK` if the function executes successfully or `ESP_FAIL` if there is an error.
extern esp_err_t oled_view_error(char* message_line);

/// @brief This function displays FFT data on an OLED screen with customizable scales.
/// @param fft_data An array with `float` values representing the FFT data to be displayed on the OLED screen.
/// @param fft_data_length The length of the FFT data array.
/// @param sample_data_length The length of the sample data used to generate the FFT data.
/// @param sample_frequency The frequency at which the signal is sampled, measured in Hz (Hertz).
/// @param y_min_magnitude_scale The minimum value for the magnitude scale on the y-axis of the FFT view.
/// @param y_max_magnitude_scale The maximum magnitude scale for the y-axis of the FFT view on the OLED display.
/// @return An `esp_err_t` type, which is either `ESP_OK` if the function executes successfully or `ESP_FAIL` if there is an error.
extern esp_err_t oled_view_fft(float* fft_data, uint32_t fft_data_length, uint32_t sample_data_length, size_t sample_frequency, float y_min_magnitude_scale, float y_max_magnitude_scale);

#endif
