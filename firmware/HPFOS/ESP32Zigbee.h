// HPFOS - firmware for the HPFC High Power Fan Controller
// Copyright (c) 2025 Marvin Ramm - PolyForm Noncommercial 1.0.0, see LICENSING.md

#ifndef ESP32ZIGBEE_H
#define ESP32ZIGBEE_H

extern void ZigbeeInit();

extern bool ZigbeeConnected();

extern void UpdateEndpoint();

#endif