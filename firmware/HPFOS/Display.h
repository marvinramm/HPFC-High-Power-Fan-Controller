// HPFOS - firmware for the HPFC High Power Fan Controller
// Copyright (c) 2025 Marvin Ramm - PolyForm Noncommercial 1.0.0, see LICENSING.md

#ifndef DISPLAY_H
#define DISPLAY_H

extern void DrawBootPage();
extern void DisplayInit();
extern void UpdateDisplay();

extern unsigned int PageNumber;
extern bool ForceDisplayUpdate;

extern unsigned int DischargePage;

extern void ClearDisplay();

#endif
