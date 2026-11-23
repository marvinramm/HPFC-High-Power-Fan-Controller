# Hardware

| File | Content |
|---|---|
| [`hpfc-v1/HPFC-V1_schematic.pdf`](hpfc-v1/HPFC-V1_schematic.pdf) | Schematic (1 sheet, A3) |
| [`hpfc-v1/HPFC-V1_BOM.csv`](hpfc-v1/HPFC-V1_BOM.csv) | Bill of materials, 74 parts |
| [`hpfc-v1/HPFC-V1_ibom.html`](hpfc-v1/HPFC-V1_ibom.html) | Interactive BOM for assembly (download and open in a browser) |
| [`hpfc-v1/source`](hpfc-v1/source) | Schematic and board as EAGLE files (Fusion Electronics 9.7) |

The fabrication data is attached to the [release](https://github.com/marvinramm/HPFC-High-Power-Fan-Controller/releases):

- **ODB++** exported from Fusion Electronics – the original data, accepted directly by most PCB manufacturers
- **Gerber + Excellon drill** generated from the EAGLE board with KiCad's EAGLE importer, including the slotted holes of the USB-C connector. Check them in a Gerber viewer before you order.

## Board

2 layers, 71.5 × 50.85 mm, 1.6 mm, four M3 mounting holes. The display, the encoder, the USB-C port and almost all parts are on the top side. The fan connectors, the 12 V terminal, the bulk capacitor C7 and the I²C header for the second sensor are on the bottom, so the board can sit directly behind a front panel.

| Block | Parts | Function |
|---|---|---|
| USB-C | J7, D1 (SP0503BAHTG) | Power input and native USB, ESD protection |
| USB-PD trigger | IC1 (CH224K) | Requests 12 V from a USB-PD charger |
| Ideal diode | IC3 (LM74700-Q1), Q1 (AO4406AL) | Connects USB-C VBUS to the 12 V rail and blocks reverse current, so an external 12 V supply never feeds back into the USB port |
| External supply | J1 (Phoenix 1711026) | 12 V screw terminal, directly on the 12 V rail |
| Fan rail switch | Q2 (AOD403, P-MOSFET), Q3 (SI2302) | Switches 12 V to all four fans (`FAN_EN`, GPIO22) |
| Fan connectors | J2–J5 (Molex 47053) | Standard 4-pin PWM fan headers: 12 V, GND, PWM (25 kHz), tach |
| Tach filter | R7–R18, C8–C11 | 10 kΩ pull-up to 3.3 V and 2.2 kΩ / 180 pF low-pass for each tach signal |
| 3.3 V | IC4 (AMS1117-3.3) | LDO from the 12 V rail, 1 A |
| MCU | IC5 (ESP32-C6-MINI-1-N4) | Zigbee / Thread, Wi-Fi 6, BLE, 4 MB flash |
| Sensors | IC2 (BME680), JP2 | BME680 on the board, second BME680 on the I²C header JP2 |
| User interface | DSP1 (SSD1306 0.96" OLED), SW1 (PEC12R encoder), LED1 (RGB) | Display, rotary encoder with push button, status LED |
| Programming | J6 (TC2050), SW2 (BOOT), SW3 (RESET) | Tag-Connect with USB D+/D−, EN and BOOT |
| Voltage sense | R2/R5, R3/R6 | Dividers for USB-C VBUS (GPIO1) and the 12 V rail (GPIO2) |

## ESP32-C6 pin assignment

| GPIO | Function | | GPIO | Function |
|---|---|---|---|---|
| 0 | LED blue | | 14–17 | PWM fan 1–4 |
| 1 | USB-C voltage (ADC) | | 18–21 | Tach fan 1–4 |
| 2 | 12 V rail voltage (ADC) | | 22 | `FAN_EN` (12 V fan rail) |
| 3 | LED red | | 23 | LED green |
| 4 | Encoder button | | 12 / 13 | USB D− / D+ |
| 5 / 8 | Encoder A / B | | 9 | BOOT button |
| 6 / 7 | I²C SDA / SCL | | | |

I²C addresses: BME680 on the board `0x77` (address set with JP1), BME680 on JP2 `0x76`, OLED `0x3C`.

## Power budget

The four fans share one 12 V rail. With four Arctic server fans at 8000 RPM the HPFC switches up to about 100 W. Use an external 12 V supply for full speed. On USB-PD the firmware limits the PWM to about 35 %, and on plain 5 V USB the fans stay off (see [firmware](../firmware/README.md#power-source-and-fan-limit)).

## License

The hardware design files and documentation are licensed under [CC BY-NC 4.0](LICENSE). Commercial use needs my agreement, see [LICENSING.md](../LICENSING.md).
