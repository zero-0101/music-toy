#pragma once

// Pins and flags: use platformio.ini build_flags when set, else these fallbacks.
// platformio.ini: I2S_BCLK/LRC/DOUT, ARDUINO_USB_MODE, USE_TINYUSB, ARDUINO_USB_CDC_ON_BOOT

#if defined(CONFIG_IDF_TARGET_ESP32S2)
    // --- ESP32-S2 Configuration ---
    #ifndef I2S_BCLK
    #define I2S_BCLK 4
    #endif
    #ifndef I2S_LRC
    #define I2S_LRC 5
    #endif
    #ifndef I2S_DOUT
    #define I2S_DOUT 6
    #endif

    #ifndef SD_CS_PIN
    #define SD_CS_PIN 34
    #endif

    #ifndef LED_PIN
    #define LED_PIN 18  // Default RGB LED on ESP32-S2-Saola-1
    #endif

#else
    // --- ESP32-S3 Configuration (Default) ---
    #ifndef I2S_BCLK
    #define I2S_BCLK 9
    #endif
    #ifndef I2S_LRC
    #define I2S_LRC 47
    #endif
    #ifndef I2S_DOUT
    #define I2S_DOUT 18
    #endif

    #ifndef SD_CS_PIN
    #define SD_CS_PIN 10
    #endif

    #ifndef LED_PIN
    #define LED_PIN 48
    #endif
#endif

#ifndef LED_COUNT
#define LED_COUNT 8
#endif
