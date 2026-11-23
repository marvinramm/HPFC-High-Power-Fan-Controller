# Licensing

**© 2025 Marvin Ramm** – https://github.com/marvinramm/HPFC-High-Power-Fan-Controller

The HPFC is source-available for **non-commercial use**. You may build it, use it, modify it and share it, as long as you credit the author and do not use it commercially. Selling boards, kits or products based on the HPFC needs my agreement first (see [Commercial use](#commercial-use)).

| Part | License | File |
|---|---|---|
| Firmware (`firmware/`, including the Zigbee2MQTT converter) | [PolyForm Noncommercial 1.0.0](https://polyformproject.org/licenses/noncommercial/1.0.0) | [`LICENSE`](LICENSE) |
| Hardware design files, images and documentation (`hardware/`, `docs/`, README, release files) | [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/) | [`hardware/LICENSE`](hardware/LICENSE) |

Note: these licenses are not "open source" in the OSI sense, because they exclude commercial use.

## Attribution

Credit the author wherever you use or share the HPFC or parts of it, e.g. in your README, in your documentation or on the PCB silkscreen. Please use this text:

> Based on "HPFC – High Power Fan Controller" by Marvin Ramm – https://github.com/marvinramm/HPFC-High-Power-Fan-Controller – licensed under CC BY-NC 4.0 / PolyForm Noncommercial 1.0.0.

Also state whether you made changes. For the firmware, keep the `Required Notice` line from [`LICENSE`](LICENSE) in every copy.

## What counts as non-commercial

Allowed without asking:

- building one or more HPFCs for yourself, your home, your workshop or your hobby projects
- use in schools, universities, non-profit organisations and for personal research
- publishing modified versions under the same terms, with attribution

Needs a commercial license:

- selling assembled boards, kits, PCBs or devices that contain the HPFC design or firmware
- using the design or firmware in a product or a paid service
- integrating it into commercial products or projects of a company

## Commercial use

Commercial use is possible by agreement, e.g. with a share of the profit. Contact me before you start:

**marvin@ramm-eng.de**

## Third-party components

The firmware uses the ESP32 Arduino core and its Zigbee library (Espressif, Apache 2.0), the Adafruit BME680, GFX and SSD1306 libraries (BSD) and the Ai Esp32 Rotary Encoder library (GPL 2.0). They are not part of this repository and keep their own licenses.

Because the rotary encoder library is GPL 2.0, no precompiled firmware binary is published. Build the firmware yourself from source (see [`firmware/README.md`](firmware/README.md)).
