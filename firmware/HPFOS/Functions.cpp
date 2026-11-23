// HPFOS - firmware for the HPFC High Power Fan Controller
// Copyright (c) 2025 Marvin Ramm - PolyForm Noncommercial 1.0.0, see LICENSING.md

///////////////////Includes//////////////////

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "AiEsp32RotaryEncoder.h"
#include "AiEsp32RotaryEncoderNumberSelector.h"

#include "Measure.h"
#include "Pins.h"
#include "Settings.h"
#include "Functions.h"
#include "ESP32Zigbee.h"
#include "Display.h"

#define LEDC_TIMER_8_BIT 8
#define LEDC_BASE_FREQ 25000

#define FAN1CH 0
#define FAN2CH 1
#define FAN3CH 2
#define FAN4CH 3

unsigned int PWRState = 0;  //0 - USB 5V, 1 - USB PD 12V, 2 - 12V Power supply

unsigned int MAXFanSpeed = 0;  //0-255

AiEsp32RotaryEncoder *rotaryEncoder = new AiEsp32RotaryEncoder(ENC_B, ENC_A, ENC_SW, -1, 8);
AiEsp32RotaryEncoderNumberSelector numberSelector = AiEsp32RotaryEncoderNumberSelector();

void IRAM_ATTR readEncoderISR() {
  rotaryEncoder->readEncoder_ISR();
}

/////////////////////////////////////////////

//////////////////Variables//////////////////

/////////////////////////////////////////////

////////////////////Code/////////////////////

void HardwareInit() {
  Serial.begin(115200);

  Serial.println("HPFC OS Init...");

  uint64_t chipid = ESP.getEfuseMac();
  char chipIDStr[13];
  sprintf(chipIDStr, "%04X%08X", (uint16_t)(chipid >> 32), (uint32_t)chipid);

  Serial.print("ChipID: ");
  Serial.println(chipIDStr);

  Wire.begin(SDA, SCL);

  DisplayInit();

  DrawBootPage();

  //Output
  pinMode(FAN_EN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  //Input
  pinMode(USBVCC_M, INPUT);
  pinMode(VCC_M, INPUT);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  pinMode(BOOT, INPUT);

  if (ZIGBEE_EN == 1) {
    ZigbeeInit();
  }

  digitalWrite(FAN_EN, LOW);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  ledcAttachChannel(FAN1, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT, FAN1CH);
  ledcAttachChannel(FAN2, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT, FAN2CH);
  ledcAttachChannel(FAN3, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT, FAN3CH);
  ledcAttachChannel(FAN4, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT, FAN4CH);

  ledcWriteChannel(FAN1CH, 0);
  ledcWriteChannel(FAN2CH, 0);
  ledcWriteChannel(FAN3CH, 0);
  ledcWriteChannel(FAN4CH, 0);

  if (BME680Init() == false) {
    Serial.println("BME680 = OFFLINE");
  } else {
    Serial.println("BME680 = ONLINE");
  }

  delay(100);

  FAN_PWR_Init();

  if (PWRState == 0) {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    delay(500);
    digitalWrite(LED_RED, HIGH);
    delay(500);
    digitalWrite(LED_RED, LOW);
    delay(500);
  } else if (PWRState == 1) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
    delay(500);
    digitalWrite(LED_BLUE, HIGH);
    delay(500);
    digitalWrite(LED_BLUE, LOW);
    delay(500);
  } else {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, HIGH);
    delay(500);
    digitalWrite(LED_GREEN, HIGH);
    delay(500);
    digitalWrite(LED_GREEN, LOW);
    delay(500);
  }

  rotaryEncoder->begin();
  rotaryEncoder->setup(readEncoderISR);
  numberSelector.attachEncoder(rotaryEncoder);

  numberSelector.setRange(0.0, MAXFanSpeed, 1, false, 0);
  numberSelector.setValue((float)STD_SPEED * 3);

  delay(2000);

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  SetFANSpeed(0);
}

void FAN_PWR_Init() {
  if (analogRead(USBVCC_M) < 1800) {
    if (analogRead(VCC_M) > 1800) {
      PWRState = 2;
      MAXFanSpeed = 255;
      digitalWrite(FAN_EN, HIGH);
    } else {
      PWRState = 0;
      MAXFanSpeed = 0;
      digitalWrite(FAN_EN, LOW);
    }
  } else {
    PWRState = 1;
    MAXFanSpeed = 90;
    digitalWrite(FAN_EN, HIGH);
  }
}

void SetFANSpeed(unsigned int speed) {
  if (speed >= MAXFanSpeed) {
    speed = MAXFanSpeed;
  }

  if (speed <= 15) {
    digitalWrite(FAN_EN, LOW);
  }else{
    digitalWrite(FAN_EN, HIGH);
  }

  ledcWriteChannel(FAN1CH, speed);
  ledcWriteChannel(FAN2CH, speed);
  ledcWriteChannel(FAN3CH, speed);
  ledcWriteChannel(FAN4CH, speed);
}

unsigned int GetRotaryNum() {
  return numberSelector.getValue();
}

bool ButtonPressed() {
  return rotaryEncoder->isEncoderButtonClicked();
}

void SetRotaryNum(unsigned int speedval) {
  numberSelector.setValue((float)speedval);
}

void PrintAllData() {
  float PressureDeltaPercent = 0;

  if (BME680Delta_Pressure == MAXPADELTA) {
    PressureDeltaPercent = 0;
  } else {
    PressureDeltaPercent = (BME680Delta_Pressure / MAXPADELTA) * 100;
  }

  Serial.print(">Temp [°C]:");
  Serial.println(BME680OUT_Temp);
  Serial.print(">Humidity [%]:");
  Serial.println(BME680OUT_Humidity);
  Serial.print(">Pressure Delta [%]:");
  Serial.println((int)PressureDeltaPercent);
  Serial.print(">Gas Resistance IN [KOhm]:");
  Serial.println(BME680IN_GasResistance);
  Serial.print(">Gas Resistance OUT [KOhm]:");
  Serial.println(BME680OUT_GasResistance);

  Serial.print(">FAN Speed [%]:");
  Serial.println(map(GetRotaryNum(), 0, 255, 0, 100));

  Serial.print(">FAN1 [rpm]:");
  Serial.println(FAN1RPM);
  Serial.print(">FAN2 [rpm]:");
  Serial.println(FAN2RPM);
  Serial.print(">FAN3 [rpm]:");
  Serial.println(FAN3RPM);
  Serial.print(">FAN4 [rpm]:");
  Serial.println(FAN4RPM);
}

/////////////////////////////////////////////
