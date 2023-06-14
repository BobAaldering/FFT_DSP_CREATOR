#ifndef DISPLAY_COMMUNICATOR_H_
#define DISPLAY_COMMUNICATOR_H_

#include <float.h>

#include "ssd1306.h"

#define DISPLAY_COMMUNICATOR_TAG ("DISPLAY_COMMUNICATOR_H_")

extern SSD1306_t oled_display;

extern void initialize_oled(size_t screen_width, size_t screen_height);

extern void oled_view_startup(char* header_line, char* information_line, char* version_line);
extern void oled_view_error(char* message);
extern void oled_view_fft(float* data, int32_t length, int width, int height, float minimum, float maximum);

#endif
