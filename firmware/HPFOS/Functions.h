// HPFOS - firmware for the HPFC High Power Fan Controller
// Copyright (c) 2025 Marvin Ramm - PolyForm Noncommercial 1.0.0, see LICENSING.md

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

extern void HardwareInit();
extern void FAN_PWR_Init();

extern unsigned int MAXFanSpeed;

extern void SetFANSpeed(unsigned int speed);

extern unsigned int GetRotaryNum();
extern bool ButtonPressed();

extern unsigned int PWRState;

extern void PrintAllData();

extern void SetRotaryNum(unsigned int speedval);

#endif
