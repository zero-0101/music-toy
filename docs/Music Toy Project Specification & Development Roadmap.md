# Project Specification: ESP32-S3 Music Toy

## 1. Project Overview

* **Goal:** A portable music toy with MP3 playback, RGB light effects, and USB music upload capability (MSC mode).
* **Developer Context:** Software Engineer based in Canada.
* **Tech Stack:** C++, ESP32-S3, 3D Printing, PlatformIO.

---

## 2. Hardware Bill of Materials (BOM)

| Category | Component | Key Specifications | Estimated Price (CNY) |
| --- | --- | --- | --- |
| **Mainboard** | ESP32-S3-DevKitC-1 | N16R8 (16MB Flash, 8MB PSRAM) | ¥35 |
| **Audio Amp** | MAX98357A | I2S Interface, 3W Class D | ¥10 |
| **Storage** | Micro SD Module | SPI with Level Shifter (74LVC125) | ¥8 |
| **Memory** | Micro SD Card | 8GB/16GB (Formatted to FAT32) | ¥20 |
| **Lighting** | WS2812B RGB LEDs | 5050 SMD, Single-wire control | ¥10 |
| **Power** | 3.7V Lipo Battery | 800-1000mAh with Protection Board | ¥15 |
| **Charging** | TP4056 Module | Type-C Input, with battery protection | ¥2 |
| **Speaker** | Full-range Speaker | 4Ω 3W (with acoustic chamber) | ¥8 |

---

## 3. Hardware Pinout Definition

| Module | Signal | ESP32-S3 GPIO | Note |
| --- | --- | --- | --- |
| **I2S Audio** | LRC / BCLK / DIN | 45 / 46 / 47 | Digital Audio Path |
| **SD Card** | CS/SCK/MOSI/MISO | 10 / 12 / 11 / 13 | SPI Interface |
| **RGB LED** | DATA | 18 | Single Pin Control (Code Default: 48 for DevKit) |
| **Buttons** | Next / Prev / Play | 1 / 2 / 3 | Internal Pull-up |
| **USB** | D- / D+ | 19 / 20 | Native S3 USB |

---

## 4. Software Configuration (`platformio.ini`)

Current working configuration (verified with PlatformIO Core 6.x):

```ini
[env:esp32-s3-devkitc-1]
; Build: pio run  (if pio not found, see docs/USE_PIO_DIRECTLY.md)
; Use standard Espressif32 platform (Arduino 2.x / IDF 4.4)
platform = espressif32 @ ^6.4.0
board = esp32-s3-devkitc-1
framework = arduino

; Serial Monitor speed
monitor_speed = 115200

; Library Dependencies
lib_deps =
    # Core Audio: High-performance I2S decoding for MP3/WAV
    # Use version 2.0.6 for compatibility with Arduino 2.x / IDF 4.x (legacy I2S)
    https://github.com/schreibfaul1/ESP32-audioI2S.git#2.0.6
    
    # LED Control: Latest 1.15.x for stable S3 RMT timing
    adafruit/Adafruit NeoPixel @ ^1.15.0
    
    # USB Stack: Version 3.7.x is required for stable MSC on ESP32-S3
    adafruit/Adafruit TinyUSB Library @ ^3.7.0

; Hardware & Build Configurations
build_flags = 
    # Enable High Speed USB (Native S3 feature)
    -D ARDUINO_USB_MODE=1
    # Use TinyUSB as the underlying stack for MSC/CDC
    -D USE_TINYUSB
    # Enable Serial Debugging through Native USB port
    -D ARDUINO_USB_CDC_ON_BOOT=1
    # Define standard audio pins to simplify code management
    -D I2S_DOUT=47
    -D I2S_BCLK=46
    -D I2S_LRC=45
    # Use C++20 dialect supported by ESP32 toolchain
    -std=gnu++2a
    # Set debug level (0=None, 1=Error, 2=Warn, 3=Info, 4=Debug, 5=Verbose)
    -D CORE_DEBUG_LEVEL=3
    # WiFiClientSecure path (needed by include/NetworkClientSecure.h when framework is 2.x)
    -I "${platformio.packages_dir}/framework-arduinoespressif32/libraries/WiFiClientSecure/src"

; Partition Scheme: Use a large app partition to accommodate the libraries
board_build.partitions = default_16MB.csv
```

---

## 5. System Logic & State Machine

The system manages the SD card control through mutual exclusion to prevent data corruption.

* **IDLE:** Standby mode, waiting for user input.
* **MUSIC_PLAYING:** ESP32 mounts SD via SPI, streams I2S data.
* **USB_DISK_MODE:** Triggered by VBUS. ESP32 unmounts SD, TinyUSB takes control for PC access.

---

## 6. Development Roadmap (6 Months)

| Phase | Description | Status |
| :--- | :--- | :--- |
| **Month 1-2 (Sourcing)** | Order hardware via 1688/Taobao. Setup Cursor + PlatformIO. | **Complete / Ongoing** |
| **Month 3 (Prototyping)** | Breadboard wiring. Test SD-to-I2S and USB-MSC switching. | **Software Implemented** |
| **Month 4 (Software)** | Finalize `SystemManager` and `AudioManager`. Add LED animations. | **Core Architecture Done** |
| **Month 5 (Mechanical)** | Design 3D enclosure in Fusion 360. Focus on acoustic chamber. | *Pending* |
| **Month 6 (Integration)** | Final assembly, soldering, and field testing. | *Pending* |

---

## 7. Technical Notes & Best Practices

*   **Capacitor:** Add 220uF between 5V/GND to buffer speaker power spikes.
*   **USB Muxing:** Ensure `SD.end()` is called before `USBDevice.detach()` to avoid filesystem errors.
*   **Acoustics:** 3D print with at least 3 walls and 20% infill for better sound resonance.
*   **Comments:** All code comments shall be in **English** to ensure cross-platform compatibility.
*   **Library Compatibility:** `ESP32-audioI2S` must be pinned to **v2.0.6** when using the standard `espressif32` platform (Arduino 2.x / IDF 4.4). Newer versions (3.x) require IDF 5.x and cause build errors with the legacy I2S driver.
*   **Build Environment:** Use `pio run` to build. If using VS Code/Cursor, ensure the PlatformIO extension is active and `compile_commands.json` is generated for IntelliSense (`pio run -t compiledb`).

---
