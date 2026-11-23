// HPFOS - firmware for the HPFC High Power Fan Controller
// Copyright (c) 2025 Marvin Ramm - PolyForm Noncommercial 1.0.0, see LICENSING.md

///////////////////Includes//////////////////

#include <Arduino.h>
#include "Zigbee.h"

#include "Measure.h"
#include "Pins.h"
#include "Settings.h"
#include "Functions.h"
#include "ESP32Zigbee.h"
#include "Display.h"



#ifndef ZIGBEE_MODE_ZCZR
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

/* Zigbee device configuration */
#define FAN_ENDPOINT         1
#define TEMP_ENDPOINT        2
#define HUMIDITY_ENDPOINT    3
#define PRESSURE_ENDPOINT    4

#define FAN_OUTPUT_MIN 0
#define FAN_OUTPUT_MAX 100

// Zigbee analog devices
ZigbeeAnalog zbFan(FAN_ENDPOINT);
ZigbeeAnalog zbTemp(TEMP_ENDPOINT);
ZigbeeAnalog zbHumidity(HUMIDITY_ENDPOINT);
ZigbeeAnalog zbPressure(PRESSURE_ENDPOINT);

/////////////////////////////////////////////

//////////////////Variables//////////////////

unsigned int speed;

/////////////////////////////////////////////

////////////////////Code/////////////////////

void onFanOutputChange(float value) {
  // value 0-100 % received from the coordinator
  SetFANSpeed(map(constrain(value, 0, 100), 0, 100, 0, 255));
  speed = constrain(value, 0, 100);
}

void ZigbeeInit() {

  uint64_t chipid = ESP.getEfuseMac();
  char chipIDStr[13];
  sprintf(chipIDStr, "%04X%08X", (uint16_t)(chipid >> 32), (uint32_t)chipid);

  // --- Zigbee Name mit Chip ID ---
  char deviceName[32];
  snprintf(deviceName, sizeof(deviceName), "HPFC_%s", chipIDStr);

  zbFan.setManufacturerAndModel("Espressif", deviceName);
  zbFan.setPowerSource(ZB_POWER_SOURCE_MAINS);

  // Fan endpoint
  zbFan.addAnalogOutput();
  zbFan.setAnalogOutputMinMax(FAN_OUTPUT_MIN, FAN_OUTPUT_MAX);
  zbFan.setAnalogOutputDescription("speed");
  zbFan.onAnalogOutputChange(onFanOutputChange);
  zbFan.setAnalogOutputResolution(1);

  // Temp endpoint
  zbTemp.addAnalogInput();
  zbTemp.setAnalogInputMinMax(0, 100);
  zbTemp.setAnalogOutputResolution(0.1);
  zbTemp.setAnalogInputDescription("temperature");

  // Humidity endpoint
  zbHumidity.addAnalogInput();
  zbHumidity.setAnalogInputMinMax(0, 100);
  zbHumidity.setAnalogOutputResolution(1);
  zbHumidity.setAnalogInputDescription("humidity");

  // Pressure endpoint
  zbPressure.addAnalogInput();
  zbPressure.setAnalogInputMinMax(0, 100);
  zbPressure.setAnalogOutputResolution(1);
  zbPressure.setAnalogInputDescription("pressure");

  // Register endpoints
  Zigbee.addEndpoint(&zbFan);
  Zigbee.addEndpoint(&zbTemp);
  Zigbee.addEndpoint(&zbHumidity);
  Zigbee.addEndpoint(&zbPressure);

  // Start Zigbee in Router mode
  if (!Zigbee.begin(ZIGBEE_ROUTER)) {
    Serial.println("Zigbee failed to start!");
    ESP.restart();
  }
  while (!Zigbee.connected()) {
    Serial.print(".");
    digitalWrite(LED_RED, LOW);
    delay(250);
    digitalWrite(LED_RED, HIGH);
    delay(250);
  }
  Serial.println("Connected");
  digitalWrite(LED_RED, HIGH);
}

bool ZigbeeConnected() {
  return Zigbee.connected();
}

void UpdateEndpoint() {

  if (digitalRead(BOOT) == LOW) {
    delay(100);
    int startTime = millis();
    while (digitalRead(BOOT) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        Serial.println("Resetting Zigbee to factory and rebooting in 1s...");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
  }

  float PressureDeltaPercent = 0;

  if (BME680Delta_Pressure == MAXPADELTA) {
    PressureDeltaPercent = 0;
  } else {
    PressureDeltaPercent = (BME680Delta_Pressure / MAXPADELTA) * 100;
  }
  // Update Zigbee endpoints
    zbTemp.setAnalogInput(BME680OUT_Temp);
    zbHumidity.setAnalogInput(BME680OUT_Humidity);
    zbPressure.setAnalogInput(PressureDeltaPercent);

    // Report
    zbTemp.reportAnalogInput();
    zbHumidity.reportAnalogInput();
    zbPressure.reportAnalogInput();
}
/////////////////////////////////////////////
