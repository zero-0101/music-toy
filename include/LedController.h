#pragma once

#include <Adafruit_NeoPixel.h>

#include "DeviceState.h"

class LedController {
 public:
  LedController(uint16_t count, uint8_t pin)
      : pixels_(count, pin, NEO_GRB + NEO_KHZ800) {}

  void begin() {
    pixels_.begin();
    pixels_.show();
    // Boot flash: brief green so you see the LED is working
    pixels_.fill(pixels_.Color(0, 24, 0));
    pixels_.show();
    delay(150);
    pixels_.clear();
    pixels_.show();
    setState(DeviceState::IDLE);
  }

  void update() {
    // Optional: add state-based animation (e.g. VU meter when PLAYING).
    // For now LED shows solid color per setState().
  }

  void setColor(uint8_t r, uint8_t g, uint8_t b) {
    pixels_.fill(pixels_.Color(r, g, b));
    pixels_.show();
  }

  void setState(DeviceState next) {
    if (next == state_) {
      return;
    }
    state_ = next;
    applyState();
  }

  DeviceState state() const { return state_; }

 private:
  void applyState() {
    switch (state_) {
      case DeviceState::IDLE:
        pixels_.clear();
        pixels_.show();
        break;
      case DeviceState::PLAYING:
        pixels_.fill(pixels_.Color(0, 32, 0));
        pixels_.show();
        break;
      case DeviceState::USB_TRANSFER:
        pixels_.fill(pixels_.Color(0, 0, 32));
        pixels_.show();
        break;
    }
  }

  Adafruit_NeoPixel pixels_;
  DeviceState state_ = DeviceState::IDLE;
};
