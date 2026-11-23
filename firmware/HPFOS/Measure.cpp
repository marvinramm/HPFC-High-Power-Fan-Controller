// HPFOS - firmware for the HPFC High Power Fan Controller
// Copyright (c) 2025 Marvin Ramm - PolyForm Noncommercial 1.0.0, see LICENSING.md

///////////////////Includes//////////////////

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#include "Measure.h"
#include "Pins.h"
#include "Settings.h"
#include "Functions.h"
#include "ESP32Zigbee.h"
#include "Display.h"

Adafruit_BME680 BME_IN(&Wire); //Inside Filter

Adafruit_BME680 BME_OUT(&Wire); //Outside Filter

/////////////////////////////////////////////

//////////////////Variables//////////////////

float BME680OUT_Temp = 0.00; //°C
float BME680Delta_Pressure = 0.00; //Pa
float BME680IN_GasResistance = 0.00; //kOhm
float BME680OUT_GasResistance = 0.00;
unsigned int BME680OUT_Humidity;

unsigned int FAN1RPM;
unsigned int FAN2RPM;
unsigned int FAN3RPM;
unsigned int FAN4RPM;

/////////////////////////////////////////////

////////////////////Code/////////////////////

bool BME680Init() {
  if (!BME_IN.begin(BME_IN_ADRESS)) {
    return false;
  } else {
    // Set up oversampling and filter initialization
    BME_IN.setTemperatureOversampling(BME680_OS_8X);
    BME_IN.setHumidityOversampling(BME680_OS_2X);
    BME_IN.setPressureOversampling(BME680_OS_4X);
    BME_IN.setIIRFilterSize(BME680_FILTER_SIZE_3);
    BME_IN.setGasHeater(320, 150);  // 320 °C for 150 ms
  }
  if (!BME_OUT.begin(BME_OUT_ADRESS)) {
    return false;
  } else {
    // Set up oversampling and filter initialization
    BME_OUT.setTemperatureOversampling(BME680_OS_8X);
    BME_OUT.setHumidityOversampling(BME680_OS_2X);
    BME_OUT.setPressureOversampling(BME680_OS_4X);
    BME_OUT.setIIRFilterSize(BME680_FILTER_SIZE_3);
    BME_OUT.setGasHeater(320, 150);  // 320 °C for 150 ms
  }
  return true;
}

void MeasureBME680() {
  if (! BME_IN.performReading()) {
    Serial.println("BME Inside Failed to perform reading");
  }
  if (! BME_OUT.performReading()) {
    Serial.println("BME Outside Failed to perform reading");
  }

  BME680OUT_Temp = BME_OUT.temperature;
  BME680Delta_Pressure = BME_OUT.pressure - BME_IN.pressure;
  BME680IN_GasResistance = BME_IN.gas_resistance / 1000.0;
  BME680OUT_GasResistance = BME_OUT.gas_resistance / 1000.0;
  BME680OUT_Humidity = BME_OUT.humidity;
}


unsigned int getRPM(unsigned int tachPin) {
  pinMode(tachPin, INPUT_PULLUP);

  unsigned long startMillis = millis();
  unsigned long pulses = 0;
  bool lastState = digitalRead(tachPin);

  while (millis() - startMillis < SAMPLE_TIME_MS) {
    bool currentState = digitalRead(tachPin);
    if (currentState != lastState) {
      pulses++;
      lastState = currentState;
    }
  }

  // edges -> full periods
  unsigned long periodChanges = pulses / 2;

  // scale to one second
  float seconds = SAMPLE_TIME_MS / 1000.0f;
  float periodPerSecond = periodChanges / seconds;

  // RPM berechnen
  unsigned int rpm = (periodPerSecond * 60.0f) / PULSES_PER_REV;
  return rpm;
}

void MeasureRPM() {
  FAN1RPM = getRPM(TACH1);
  FAN2RPM = getRPM(TACH2);
  FAN3RPM = getRPM(TACH3);
  FAN4RPM = getRPM(TACH4);
}

/////////////////////////////////////////////
