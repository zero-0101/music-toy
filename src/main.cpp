#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include "BoardConfig.h"
#include "LedController.h"
#include "MusicPlayer.h"

// --- RGB Strip (MOSFETs) ---
// Note: We use manual PWM for now because the hardware uses MOSFETs (Pins 16, 17, 18),
// while LedController class is designed for addressable WS2812B LEDs.
const int PIN_R = 16;
const int PIN_G = 17;
const int PIN_B = 18;

const int CH_R = 0;
const int CH_G = 1;
const int CH_B = 2;

// --- Buttons (9 keys) ---
// Using GPIO 7-15
const int BUTTON_PINS[9] = {7, 8, 9, 10, 11, 12, 13, 14, 15};
const int DEBOUNCE_MS = 50;

struct Button {
  int pin;
  int state;
  int lastReading;
  unsigned long lastDebounceTime;
};

Button buttons[9];

// --- Application State ---
bool sdFound = false;
unsigned long lastLedUpdate = 0;

// --- Helper Functions ---

void setColor(int r, int g, int b) {
  ledcWrite(CH_R, r);
  ledcWrite(CH_G, g);
  ledcWrite(CH_B, b);
}

void initButtons() {
  for (int i = 0; i < 9; i++) {
    buttons[i].pin = BUTTON_PINS[i];
    pinMode(buttons[i].pin, INPUT_PULLUP);
    buttons[i].state = HIGH;  // Default high (pullup)
    buttons[i].lastReading = HIGH;
    buttons[i].lastDebounceTime = 0;
  }
}

void checkButtons() {
  for (int i = 0; i < 9; i++) {
    int reading = digitalRead(buttons[i].pin);

    if (reading != buttons[i].lastReading) {
      buttons[i].lastDebounceTime = millis();
    }

    if ((millis() - buttons[i].lastDebounceTime) > DEBOUNCE_MS) {
      if (reading != buttons[i].state) {
        buttons[i].state = reading;
        if (buttons[i].state == LOW) {  // Button Pressed
          Serial.printf("Button %d pressed (GPIO %d)\n", i + 1, buttons[i].pin);
          
          if (!sdFound) {
            Serial.println("Error: No SD Card found. Cannot play music.");
            // Blink red to indicate error
            setColor(255, 0, 0);
            delay(200);
            setColor(0, 0, 0);
            delay(200);
            return;
          }

          // Logic for Button 1
          if (i == 0) { // Button 1
             Serial.println("-> Playing /file1.mp3");
             MusicPlayer::getInstance().play("/file1.mp3");
          }
          else if (i == 1) { // Button 2 - Stop
             Serial.println("-> Stop");
             MusicPlayer::getInstance().stop();
          }
        }
      }
    }
    buttons[i].lastReading = reading;
  }
}

void updateLEDs() {
  if (MusicPlayer::getInstance().audio().isRunning()) {
    // "Dance": Random color every 100ms
    if (millis() - lastLedUpdate > 100) {
      int r = random(0, 256);
      int g = random(0, 256);
      int b = random(0, 256);
      setColor(r, g, b);
      lastLedUpdate = millis();
    }
  } else {
    // Idle: Slow blue breathing
    unsigned long ms = millis();
    float breath = (exp(sin(ms / 2000.0 * PI)) - 0.36787944) * 108.0;
    setColor(0, 0, (int)breath);
  }
}

// --- Main ---

void setup() {
  Serial.begin(115200);
  
  // Setup PWM
  ledcSetup(CH_R, 5000, 8);
  ledcSetup(CH_G, 5000, 8);
  ledcSetup(CH_B, 5000, 8);
  ledcAttachPin(PIN_R, CH_R);
  ledcAttachPin(PIN_G, CH_G);
  ledcAttachPin(PIN_B, CH_B);

  initButtons();

  // Initialize MusicPlayer (I2S)
  Serial.println("Initializing Audio...");
  MusicPlayer::getInstance().begin();
  MusicPlayer::getInstance().audio().setVolume(21); // Set default volume (0-21)

  // Initialize SD Card
  Serial.print("Initializing SD card...");
  // Use default VSPI pins or configure explicitly if needed
  // For ESP32-S2 Saola, default VSPI is often: SCK=12, MISO=13, MOSI=11, SS=10(or 34)
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("FAILED!");
    Serial.println("Make sure SD card is inserted and wired correctly.");
    sdFound = false;
  } else {
    Serial.println("OK.");
    sdFound = true;
  }

  Serial.println("\n=== ESP32 Phone Toy ===");
  Serial.println("Buttons 1-9 on GPIO 7-15");
}

void loop() {
  checkButtons();
  updateLEDs();
  
  // Keep audio system running
  MusicPlayer::getInstance().update();
}
