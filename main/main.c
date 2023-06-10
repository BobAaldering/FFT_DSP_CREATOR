// Several useful links for DSP on the ESP32:
// 1. https://espressif-docs.readthedocs-hosted.com/projects/esp-dsp/en/latest/esp-dsp-apis.html
// 2. https://github.com/espressif/esp-dsp
// 3. https://github.com/espressif/esp-dsp/issues/21
// 4. https://github.com/espressif/esp-dsp/issues/4

#include "fft_transform.h"

void app_main() {
    fft_data_t data_for_fft = {};

    wave_config_t data_for_waves[] = {
        {
            .amplitude = 1.0,
            .frequency = 0.25,
            .phase = 0.0
        },
        {
            .amplitude = 0.5,
            .frequency = 0.1,
            .phase = 0.0
        },
    };

    initialize_fft_data(&data_for_fft);
    initialize_fft_f32(&data_for_fft);

    generate_waves_f32(data_for_waves, &data_for_fft, sizeof(data_for_waves) / sizeof(data_for_waves[0]));

    apply_fft_f32(&data_for_fft, HANN_WINDOW_F32);

    de_initialize_fft_f32(&data_for_fft);
    de_initialize_fft_data(&data_for_fft);
}
