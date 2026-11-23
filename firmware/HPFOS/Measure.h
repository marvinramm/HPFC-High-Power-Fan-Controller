// HPFOS - firmware for the HPFC High Power Fan Controller
// Copyright (c) 2025 Marvin Ramm - PolyForm Noncommercial 1.0.0, see LICENSING.md

#ifndef MEASURE_H
#define MEASURE_H

extern bool BME680Init();
extern void MeasureBME680();

extern float BME680OUT_Temp;
extern float BME680Delta_Pressure;
extern float BME680IN_GasResistance;
extern float BME680OUT_GasResistance;
extern unsigned int BME680OUT_Humidity;

extern void MeasureRPM();

extern unsigned int FAN1RPM;
extern unsigned int FAN2RPM;
extern unsigned int FAN3RPM;
extern unsigned int FAN4RPM;

#endif
