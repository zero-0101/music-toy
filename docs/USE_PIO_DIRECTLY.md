# Using `pio` directly

To run `pio run`, `pio run -t fullclean`, etc. without typing `python3 -m platformio`, install PlatformIO and add its executable directory to your PATH.

## 1. Install PlatformIO

```bash
python3 -m pip install --user platformio
```

(Or use a venv: `python3 -m venv .venv && source .venv/bin/activate` then `pip install platformio`.)

## 2. Add `pio` to PATH

After install, the `pio` script is in a “bin” directory. Add that directory to `PATH` in your shell config (e.g. `~/.zshrc`):

**macOS (Homebrew Python):**
```bash
export PATH="$HOME/Library/Python/3.14/bin:$PATH"
```
(Use your actual Python version, e.g. `3.11`, `3.12`, instead of `3.14` if different.)

**macOS / Linux (pip --user default):**
```bash
export PATH="$HOME/.local/bin:$PATH"
```

Then reload your shell or open a new terminal:
```bash
source ~/.zshrc
```

## 3. Verify

```bash
pio --version
pio run
```

You can then use `pio` directly: `pio run`, `pio run -t fullclean`, `pio run -t compiledb`, etc.

---

## 4. Cheat Sheet for ESP32-S2 Development

### **Build & Upload**

| Action | Command | Note |
| :--- | :--- | :--- |
| **Build** | `pio run` | Compiles the code. |
| **Upload** | `pio run -t upload` | Builds and uploads to the board. |
| **Upload (Specific Port)** | `pio run -t upload --upload-port /dev/cu.usbserial-1130` | If auto-detect fails. Replace port name as needed. |
| **Clean Build** | `pio run -t clean` | Removes build files (fix weird cache issues). |

### **Monitoring & Debugging**

| Action | Command | Note |
| :--- | :--- | :--- |
| **Serial Monitor** | `pio device monitor` | View logs (`Serial.print`). Ctrl+C to exit. |
| **List Ports** | `ls /dev/cu.*` | Check what ports are available on macOS. |

### **Troubleshooting Uploads**

If upload fails (e.g. "Serial data stream stopped", "No serial data received"):

1.  **Try BOOT+RESET:** Hold **BOOT**, press **RESET**, release both. Then run upload immediately.
2.  **Port Busy?** Ensure Serial Monitor is CLOSED before uploading.
3.  **Slower upload:** In `platformio.ini`, set `upload_speed = 115200` or `57600`.
4.  **Different cable/port:** Use a data-capable USB cable and try another USB port.

### **Configuration (platformio.ini)**

*   **USB Mode:** `ARDUINO_USB_MODE=0` (OTG for S2/S3).
*   **CDC on Boot:** `ARDUINO_USB_CDC_ON_BOOT=1` (Enables Serial over USB).
*   **Libraries:** Check `lib_deps` for pinned versions (e.g. `ESP32-audioI2S` is pinned to avoid breaking changes).

---

## If you see `driver/i2s_std.h: No such file or directory`

That header is part of **ESP-IDF 5.x**. The default PlatformIO Arduino-ESP32 framework (2.0.x / 3.2xxxx) is built with **ESP-IDF 4.x**, which does not include `driver/i2s_std.h`.

This project is configured to use **Arduino-ESP32 3.0.x** with **ESP-IDF 5.1** via:

- **Platform:** `platform-espressif32` from branch `develop` (supports `framework-arduinoespressif32-libs`).
- **platform_packages:** Arduino-ESP32 3.0.7 and `esp32-arduino-libs` for IDF 5.1.

On first build, PlatformIO will clone the platform and the two framework packages; the build may take longer. If you still see the error, run:

```bash
pio pkg update
pio run -t fullclean
pio run
```

To go back to the default platform (no IDF 5), remove the `platform` and `platform_packages` overrides from `platformio.ini` and use an older ESP32-audioI2S version that supports the legacy I2S driver, if available.
