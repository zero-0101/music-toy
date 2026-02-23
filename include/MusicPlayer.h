#pragma once

#include <Audio.h>
#include <cstdint>

#include "BoardConfig.h"
#include "DeviceState.h"

/**
 * I2S pin configuration for the ESP32-audioI2S backend.
 * Defaults come from platformio.ini build_flags: I2S_BCLK, I2S_LRC, I2S_DOUT.
 *
 * The library routes these GPIOs through the ESP-IDF I2S driver (i2s_std_config_t).
 * Pins are claimed when the Audio object starts playback (I2Sstart).
 *
 * - BCLK:  Bit clock (SCK).  LRC: Word select (WS).  DOUT: Data to DAC.  MCLK: optional (-1).
 */
struct I2SPinConfig {
  uint8_t bclk;
  uint8_t lrc;
  uint8_t dout;
  int8_t mclk;

  I2SPinConfig(uint8_t b, uint8_t l, uint8_t d, int8_t m = -1)
      : bclk(b), lrc(l), dout(d), mclk(m) {}
};

/**
 * Singleton music player that encapsulates the ESP32-audioI2S library.
 *
 * Design notes:
 * - Audio is held by value (not unique_ptr) to avoid heap use and match common
 *   embedded style; the object is created once with the singleton.
 * - Raw pointers are used only when the library API requires it (e.g. connecttohost);
 *   ownership stays inside this class.
 */
class MusicPlayer {
 public:
  MusicPlayer(const MusicPlayer&) = delete;
  MusicPlayer& operator=(const MusicPlayer&) = delete;
  MusicPlayer(MusicPlayer&&) = delete;
  MusicPlayer& operator=(MusicPlayer&&) = delete;

  /** Returns the single instance. Thread-unsafe; call from one task/thread only. */
  static MusicPlayer& getInstance() {
    static MusicPlayer instance;
    return instance;
  }

  /**
   * Initializes the player and applies I2S pin routing.
   * Default pins use I2S_BCLK, I2S_LRC, I2S_DOUT from platformio.ini when defined.
   */
  void begin(const I2SPinConfig& pins = {I2S_BCLK, I2S_LRC, I2S_DOUT, -1}) {
    pin_config_ = pins;
    audio_.setPinout(pins.bclk, pins.lrc, pins.dout, pins.mclk);
    setState(DeviceState::IDLE);
  }

  void update() {
    switch (state_) {
      case DeviceState::IDLE:
        break;
      case DeviceState::PLAYING:
        audio_.loop();
        break;
      case DeviceState::USB_TRANSFER:
        break;
    }
  }

  void play(const char* url) {
    if (!url) return;
    audio_.connecttohost(url);
    setState(DeviceState::PLAYING);
  }

  void stop() {
    audio_.stopSong();
    setState(DeviceState::IDLE);
  }

  [[nodiscard]] DeviceState state() const { return state_; }
  [[nodiscard]] const I2SPinConfig& pinConfig() const { return pin_config_; }

  /** Raw reference for advanced use (e.g. volume, callbacks). Prefer public API. */
  Audio& audio() { return audio_; }
  const Audio& audio() const { return audio_; }

 private:
  MusicPlayer() = default;

  void setState(DeviceState next) {
    if (next == state_) return;
    onEnterState(next);
    state_ = next;
  }

  void onEnterState(DeviceState next) {
    switch (next) {
      case DeviceState::IDLE:
        break;
      case DeviceState::PLAYING:
        break;
      case DeviceState::USB_TRANSFER:
        audio_.stopSong();
        break;
    }
  }

  Audio audio_;
  I2SPinConfig pin_config_{I2S_BCLK, I2S_LRC, I2S_DOUT, -1};
  DeviceState state_ = DeviceState::IDLE;
};
