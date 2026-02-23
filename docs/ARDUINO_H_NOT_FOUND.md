# Why "Arduino.h file not found" Appears

## 1. It's from the IDE, not the compiler

The message comes from the **editor's C/C++ engine** (IntelliSense, clangd, etc.), not from a real build.

- **`pio run`** uses PlatformIO's own flags and includes. The framework path is added automatically, so the **compiler** finds `Arduino.h` and the build can succeed.
- The **IDE** uses its own config (e.g. `.vscode/c_cpp_properties.json` or `compile_commands.json`). It does **not** run PlatformIO, so it has no built‑in idea where the ESP32 Arduino core lives. If that config doesn't list the framework path, you get **"'Arduino.h' file not found"** in the editor even when `pio run` works.

So the problem is: **wrong or incomplete include paths for the IDE**, not wrong code.

---

## 2. Two worlds of "includes"

| Aspect | Real build (`pio run`) | IDE (IntelliSense / clangd) |
|--------|------------------------|-----------------------------|
| Who runs it | PlatformIO + toolchain | VS Code / Cursor C++ extension or clangd |
| Where includes come from | PlatformIO env (platform + framework + libs) | `c_cpp_properties.json` or `compile_commands.json` |
| `#include <Arduino.h>` | Resolved by `-I.../cores/esp32` from framework | Resolved only if that path is in IDE config |

If the IDE config never adds the framework's `cores/esp32` (and optionally `variants/esp32s3`) directory, the IDE never sees `Arduino.h` and reports "file not found."

---

## 3. Why your `.vscode` config can still "fail"

`.vscode/c_cpp_properties.json` currently points at paths like:

- `~/.platformio/platforms/espressif32/framework-arduinoespressif32/cores/esp32`
- `~/.platformio/packages/framework-arduinoespressif32/cores/esp32`

It can still show "Arduino.h file not found" if:

1. **Path doesn't exist on your machine**  
   Framework can be under `platforms/` or `packages/`, and names can change with platform/framework version. If that exact path isn't there, the IDE never finds `Arduino.h`.

2. **Wrong configuration selected**  
   The dropdown (e.g. "Win32", "Mac", "ESP32-S3 (PlatformIO)") must use the configuration that has these `includePath` entries. If another config is active, you get no framework path → "file not found".

3. **Environment variables not what you expect**  
   `includePath` uses `${env:HOME}` or `${env:USERPROFILE}`. In Cursor/remote/container setups these can differ from your normal shell, so the expanded path may not match your real `.platformio` location.

4. **Tool/extension uses `compile_commands.json`**  
   If you use clangd or "compile commands" mode, the IDE may **ignore** `c_cpp_properties.json` and only look at `compile_commands.json`. If that file is missing or not generated from the PlatformIO build, the IDE again has no framework path.

---

## 4. What to do

- **Confirm it's IDE-only**  
  Run `pio run` (or `python3 -m platformio run` if `pio` is not in PATH). If the build succeeds, the "Arduino.h file not found" you see is from the IDE, not from the compiler.

- **Use the right configuration**  
  In the status bar, pick the config that includes the ESP32 framework paths (e.g. "ESP32-S3 (PlatformIO)").

- **Match the real framework path**  
  After `pio run`, the framework is under your PlatformIO install. Find it, e.g.:  
  `find ~/.platformio -name "Arduino.h" -path "*/esp32/*"`  
  Put the **directory that contains** `Arduino.h` into `includePath` in `c_cpp_properties.json` (and the variants path if you use it).

- **Prefer compile_commands from the build**  
  Run:  
  ```bash
  pio run -t compiledb
  # Or if pio is not in PATH:
  python3 -m platformio run -t compiledb
  ```  
  and, if your IDE/clangd is set to use it, point it at the generated `compile_commands.json`. That makes the IDE use the same include paths as the real build, which removes "Arduino.h file not found" as long as the build succeeds.

---

## 5. PlatformIO Not Installed? (`pio: command not found`)

If you see `zsh: command not found: pio`:

1. **Install PlatformIO Core:**
   ```bash
   python3 -m pip install --user platformio
   # Then add to PATH (add to ~/.zshrc):
   export PATH="$HOME/.local/bin:$PATH"
   # Or on macOS, might be:
   export PATH="$HOME/Library/Python/3.x/bin:$PATH"
   ```

2. **Or use Python module directly (no PATH needed):**
   ```bash
   python3 -m platformio run
   python3 -m platformio run -t compiledb
   ```

3. **Or install PlatformIO IDE extension** in Cursor/VS Code, which provides `pio` via the extension environment.

---

## 6. Summary

| Cause | Meaning |
|-------|--------|
| IDE doesn't know framework path | "'Arduino.h' file not found" in editor; `pio run` can still succeed. |
| Wrong or outdated paths in `c_cpp_properties.json` | Paths don't match your PlatformIO install. |
| Wrong config / no `compile_commands.json` | IDE isn't using any config that has the framework include path. |
| PlatformIO not installed | `pio: command not found` - install PlatformIO or use `python3 -m platformio`. |

The code and `#include <Arduino.h>` are correct. The fix is to give the **IDE** the same framework include path that **PlatformIO** uses when it runs the compiler.
