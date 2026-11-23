// HPFOS - firmware for the HPFC High Power Fan Controller
// Copyright (c) 2025 Marvin Ramm - PolyForm Noncommercial 1.0.0, see LICENSING.md

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Measure.h"
#include "Pins.h"
#include "Settings.h"
#include "Functions.h"
#include "ESP32Zigbee.h"
#include "Display.h"


////////////Variables//////////////

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned int PageNumber = 0;

bool ForceDisplayUpdate = false;

/////////////////////////////////

void ClearDisplay() {
  display.clearDisplay();
  display.display();
}

void DisplayInit() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();
}

void DrawBootPage() {
  display.clearDisplay();
  display.display();

  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(29, 11);
  display.println("HPFC");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 45);
  display.println("Dev. by Marvin Ramm");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 55);
  display.print("V");
  display.println(VERSION);

  display.display();
  delay(1200);
}

void UpdateDisplay() {
  display.clearDisplay();

  if (PageNumber == 0) {

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);

    if (PWRState == 0) {
      display.println("USB 5V");
    } else if (PWRState == 1) {
      display.println("USB PD 12V");
    } else {
      display.println("Internal Supply");
    }

    if (ZigbeeConnected()) {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(115, 0);
      display.println("ZB");
    }

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 12);

    display.println("FAN Speed");

    display.setTextSize(3);
    display.setTextColor(WHITE);
    if (map(GetRotaryNum(), 0, 255, 0, 100) < 10) {
      display.setCursor(45, 31);
    } else if ((map(GetRotaryNum(), 0, 255, 0, 100) < 100) && (map(GetRotaryNum(), 0, 255, 0, 100) >= 10)) {
      display.setCursor(36, 31);
    } else {
      display.setCursor(27, 31);
    }
    display.print(map(GetRotaryNum(), 0, 255, 0, 100));
    display.println("%");

  } else {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);

    if (PWRState == 0) {
      display.println("USB 5V");
    } else if (PWRState == 1) {
      display.println("USB PD 12V");
    } else {
      display.println("Internal Supply");
    }

    if (ZigbeeConnected()) {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(115, 0);
      display.println("ZB");
    }

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 12);

    display.println("Filter Pressure");

    float PressureDeltaPercent = 0;

    if (BME680Delta_Pressure == MAXPADELTA){
      PressureDeltaPercent = 0;
    }else {
      PressureDeltaPercent = (BME680Delta_Pressure/MAXPADELTA)*100;
    }

    display.setTextSize(3);
    display.setTextColor(WHITE);
    if (PressureDeltaPercent < 10) {
      display.setCursor(45, 31);
    } else if ((PressureDeltaPercent < 100) && (PressureDeltaPercent >= 10)) {
      display.setCursor(36, 31);
    } else {
      display.setCursor(27, 31);
    }
    display.print((int)PressureDeltaPercent);
    display.println("%");

  }

  display.display();
}
