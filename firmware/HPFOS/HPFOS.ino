/////////////////////////////////////////////
//          HPFOS by Marvin Ramm           //
//               REV: V0.1.4               //
//   github.com/marvinramm/HPFC-High-      //
//          Power-Fan-Controller           //
//                                         //
//   Copyright (c) 2025 Marvin Ramm        //
//   PolyForm Noncommercial 1.0.0          //
//   Commercial use: see LICENSING.md      //
/////////////////////////////////////////////

/*
   Arduino ESP32-C6 Settings:
   Board: "ESP32C6 Dev Module"
   Upload Speed: "921600"
   CPU Frequency: "240MHz (WiFi/BT)"
   Flash Frequency: "80MHz"
   Flash Mode: "QIO"
   Flash Size: "4MB"
   Partition Scheme: "Zigbee ZCZR 4MB"
   Zigbee Mode: "Zigbee ZCZR"
   Programmer: "AVRISP mkll"
*/

/*
   Planned improvements:
   - Power-on hours counter
   - Calibration of the differential pressure
   - Warn when a fan stops (RPM monitoring)
   - Interrupt-based button handling
*/


///////////////////Includes//////////////////

#include "Measure.h"
#include "Pins.h"
#include "Settings.h"
#include "Functions.h"
#include "ESP32Zigbee.h"
#include "Display.h"

//////////////////Variables//////////////////

unsigned long PreviousMillis1;
unsigned long PreviousMillis2;
unsigned long LastInteractionMillis = 0;
unsigned long PreviousMillis3;

unsigned int LastRotaryNum;

/////////////////////////////////////////////

////////////////////Setup////////////////////

void setup() {
  HardwareInit();
  LastInteractionMillis = millis();  // start of the idle timer
}

/////////////////////////////////////////////

////////////////////Loop/////////////////////

void loop() {

  if (ButtonPressed()) {
    PageNumber++;
    if (PageNumber >= 2) {
      PageNumber = 0;
    }
    ForceDisplayUpdate = true;
    LastInteractionMillis = millis();
    delay(200);
  }

  LastRotaryNum = GetRotaryNum();

  if ((millis() - PreviousMillis1 >= 2000) && (ForceDisplayUpdate == false)) {
    PreviousMillis1 = millis();
    MeasureBME680();

    MeasureRPM();

    float PressureDeltaPercent = 0;
    if (BME680Delta_Pressure == MAXPADELTA) {
      PressureDeltaPercent = 0;
    } else {
      PressureDeltaPercent = (BME680Delta_Pressure / MAXPADELTA) * 100;
    }

    if (PressureDeltaPercent < 45) {
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, HIGH);
    } else if ((PressureDeltaPercent >= 45) && (PressureDeltaPercent < 70)) {
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, HIGH);
    } else {
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_BLUE, HIGH);
    }

    PrintAllData();
  }

  bool isUserActive;

  if (LastRotaryNum == GetRotaryNum()) {
    isUserActive = (millis() - LastInteractionMillis < 200000);
  } else {
    isUserActive = true;
    LastInteractionMillis = millis();
  }

  if (((millis() - PreviousMillis2 >= 500) || (ForceDisplayUpdate == true)) && isUserActive == true) {
    PreviousMillis2 = millis();
    //SetFANSpeed(GetRotaryNum());
    UpdateDisplay();
    ForceDisplayUpdate = false;
  }

  if ((isUserActive == false) && (millis() - PreviousMillis2 >= 500)) {
    PreviousMillis2 = millis();
    ClearDisplay();
  }

  if (millis() - PreviousMillis3 >= 30000) {
    PreviousMillis3 = millis();
    UpdateEndpoint();
  }

}

/////////////////////////////////////////////