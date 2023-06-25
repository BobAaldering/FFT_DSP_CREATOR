# FFT DSP CREATOR

_See the license of the code (copyright), please **do not** use the code for educational purposes._

## Study on Digital Signal Processing

This project includes the implementation of a program for the ESP32 microcontrollers, which will serve as a spectrum analyzer. The microcontroller hereby operates as an HTTP server, to which data can be sent. This can be a list of waves, to which the FFT can be applied to visualize the entire spectrum on an OLED display.

The HTTP server on the ESP32 can be contacted via the following URIs:

- `/wave`. This URI is used to send a list of waves to the ESP32. The waves represent different audio frequencies with their corresponding properties such as amplitude, frequency, phase, and offset.

- `/fft`. This URI triggers the Fast Fourier Transform (FFT) operation on the received wave data. The ESP32 will apply the FFT algorithm to the stored wave samples and calculate the frequency spectrum. The resulting spectrum data will be displayed on the OLED display.

- `/dac`. This URI is used to output the digital samples (created with the `/wave` URI) to the DAC (Digital-to-Analog Converter). The digital samples represent the waveform obtained after applying the FFT. The ESP32 will convert these digital samples to analog signals and output them through the DAC.

## Example usage

Below are examples of how the URIs can be called via a command prompt, along with an outline of the data that can be sent. Examples are given for two platforms, namely Linux and Windows (specifically PowerShell in that case).

- The application of the `/wave` URI:

    **On Linux:**
    ```shell
    curl -X POST -H "Content-Type: application/json" -d '{"sample_frequency": 200, "waves": [{"amplitude": 1.65, "frequency": 60, "phase": 0.0, "offset": 0.0}, {"amplitude": 0.75, "frequency": 80, "phase": 0.0, "offset": 0.0}]}' http://xxx.xxx.x.xx/wave
    ```

    **On Windows:**
    ```powershell
    Invoke-RestMethod -Uri "http://xxx.xxx.x.xx/wave" -Method POST -Headers @{"Content-Type"="application/json"} -Body '{"sample_frequency": 200, "waves": [{"amplitude": 1.65, "frequency": 60, "phase": 0.0, "offset": 0.0}, {"amplitude": 0.75, "frequency": 80, "phase": 0.0, "offset": 0.0}]} 
    ```

- The application of the `/fft` URI:

    **On Linux:**
    ```shell
    curl -X POST -H "Content-Type: application/json" -d '{"window": "HANN_F32"}' http://xxx.xxx.x.xx/fft 
    ```

    **On Windows:**
    ```powershell
    Invoke-RestMethod -Uri "http://xxx.xxx.x.xx/fft" -Method POST -Headers @{"Content-Type"="application/json"} -Body '{"window": "HANN_F32"}'
    ```

- The application of the `/dac` URI:

    **On Linux:**
    ```shell
    curl -X POST -H "Content-Type: application/json" -d '{"prevent_overflow_value": true}' http://xxx.xxx.x.xx/dac
    ```

    **On Windows:**
    ```powershell
    Invoke-RestMethod -Uri "http://xxx.xxx.x.xx/fft" -Method POST -Headers @{"Content-Type"="application/json"} -Body '{"prevent_overflow_value": true}'
    ```

## Project setup

In order to be able to work with this project in ESP-IDF, a number of steps must first be taken. This is due to the fact that various components and source files have been omitted (eg libraries and a header file with the Wi-Fi data). The steps are explained below to be able to build, compile and upload the project yourself:

- This repository will need to be cloned, which can be done with the following command:
    ```shell
    git clone https://github.com/BobAaldering/FFT_DSP_CREATOR
    ```

- Then the project will have to be opened in an IDE. It is recommended to use Visual Studio Code, in combination with the ESP-IDF plugin. Once installed (together with the IDF) all windows will be opened, including compiling and uploading.

- The functions given in the official `esp_dsp` library will have to be obtained. That can be done via `idf.py`. Via Visual Studio Code you first have to open the `command palette` and then type in `Open ESP-IDF Terminal`. A terminal will open, where you can type the following:
    ```shell
    idf.py add-dependency "espressif/esp-dsp"
    ```

- The library for the OLED display will have to be obtained. An `SSD1306` type display is used. For ESP-IDF, there is a library for this that is publicly available on [GitHub](https://github.com/nopnop2002/esp-idf-ssd1306). You must first create a folder `components` in your project directory, and then clone it there:
    ```shell
    ../components/ git clone https://github.com/nopnop2002/esp-idf-ssd1306
    ```
    Be aware that after cloning two functions still need to be adjusted, that is `_ssd1306_pixel` and `_ssd1306_line`! Change the names to `ssd1306_pixel` and `ssd1306_line`.

- In the `main` folder, a header file must be added, in which all Wi-Fi data must be entered according to your own SSID and password. The file should be called `wifi_info.h` and contain the following information:
    ```c
    #ifndef WIFI_PASS_H_
    #define WIFI_PASS_H_

    #define SSID_NAME ("YOUR_SSID_NAME")
    #define PASS_NAME ("YOUR_PASS_NAME")

    #endif
    ```

To ensure that the project is executed on the ESP32, you can simply type the following via the `command palette`: `ESP-IDF: Build, Flash and start a monitor on your device`. Set the correct COM port and upload method.

## Useful links

Below are a number of useful links that provide a detailed explanation of how to use the 'FFT' within the official `esp_dsp` library:

1. The complete [documentation](https://espressif-docs.readthedocs-hosted.com/projects/esp-dsp/en/latest/esp-dsp-apis.html) of `esp_dsp`.
2. A link to the [GitHub repository](https://github.com/espressif/esp-dsp) of `esp_dsp`.
3. A complete [explanation/discussion](https://github.com/espressif/esp-dsp/issues/4) how to use the 'FFT' in general on the ESP32.
4. An [explanation](https://github.com/espressif/esp-dsp/issues/21) how to retrieve the actual frequencies of the peak amplitudes from a `Bin` (scale generated by 'FFT').
