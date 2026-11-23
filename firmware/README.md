# HPFOS – HPFC firmware

Firmware V0.1.4 for the ESP32-C6 on the HPFC board, written with the ESP32 Arduino core.

| File | Content |
|---|---|
| `HPFOS.ino` | Main loop: reads the sensors every 2 s, sets the status LED, updates the display and reports to Zigbee every 30 s |
| `Functions.cpp` | Hardware init, power source detection, 25 kHz fan PWM, rotary encoder |
| `Measure.cpp` | Both BME680 sensors, pressure difference, fan RPM from the tach signals |
| `Display.cpp` | SSD1306 OLED pages (fan speed, filter pressure) and display timeout |
| `ESP32Zigbee.cpp` | Zigbee router with four analog endpoints, factory reset |
| `Pins.h` / `Settings.h` | Pin assignment and settings |
| [`zigbee2mqtt/HPFC.js`](zigbee2mqtt/HPFC.js) | External converter for Zigbee2MQTT |

## Build

Requirements:

- [ESP32 Arduino core](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) **3.3.x** (tested with 3.3.3; the analog Zigbee endpoints need 3.3 or newer)
- Libraries: Adafruit BME680, Adafruit Unified Sensor, Adafruit GFX, Adafruit SSD1306, Ai Esp32 Rotary Encoder

Arduino IDE settings:

| Setting | Value |
|---|---|
| Board | ESP32C6 Dev Module |
| Flash size | 4 MB |
| Partition scheme | Zigbee ZCZR 4MB with spiffs |
| Zigbee mode | Zigbee ZCZR (coordinator/router) |

With `arduino-cli`:

```bash
arduino-cli compile --upload -p /dev/ttyUSB0 \
  --fqbn esp32:esp32:esp32c6:PartitionScheme=zigbee_zczr,ZigbeeMode=zczr \
  firmware/HPFOS
```

Flash over USB-C: the data lines go to the native USB of the ESP32-C6 (GPIO12/13). The Tag-Connect footprint J6 (TC2050) carries the same USB lines plus EN and BOOT. If the upload does not start, hold the BOOT button (SW2), press RESET (SW3) and release BOOT.

## Settings (`Settings.h`)

| Define | Default | Meaning |
|---|---|---|
| `MAXPADELTA` | 350 | Pressure difference across the filter in Pa that counts as 100 % filter load |
| `ZIGBEE_EN` | 1 | Zigbee on/off |
| `STD_SPEED` | 0 | Start value of the encoder |
| `PULSES_PER_REV` | 2 | Tach pulses per fan revolution |
| `SAMPLE_TIME_MS` | 200 | Sampling window for the RPM measurement |

## Power source and fan limit

At start-up the firmware measures the USB-C voltage (GPIO1) and the 12 V rail (GPIO2) through voltage dividers:

| Source | Display | Max. PWM | Start-up LED |
|---|---|---|---|
| USB-C without PD (5 V) | `USB 5V` | 0 – fans stay off | red |
| USB-C PD 12 V | `USB PD 12V` | 90 / 255 (≈ 35 %), to stay within the PD power budget | blue |
| External 12 V supply | `Internal Supply` | 255 (100 %) | green |

Below a PWM value of 16 the 12 V fan rail is switched off completely (`FAN_EN`), so the fans stop instead of idling.

## Zigbee

The HPFC joins an existing Zigbee network as a router (mains powered). At start-up it waits for the network and blinks red until it has joined. Hold `BOOT` (GPIO9) for 3 s to reset the Zigbee settings and join again.

| Endpoint | Cluster | Value |
|---|---|---|
| 1 | Analog Output | Fan speed 0–100 % (writable) |
| 2 | Analog Input | Temperature in °C |
| 3 | Analog Input | Relative humidity in % |
| 4 | Analog Input | Filter load in % of `MAXPADELTA` |

The model ID is `HPFC_<chip ID>`, so every board reports its own ID.

### Zigbee2MQTT

1. Copy [`zigbee2mqtt/HPFC.js`](zigbee2mqtt/HPFC.js) into the `external_converters` folder of your Zigbee2MQTT data directory.
2. Pair the HPFC. Zigbee2MQTT shows its model ID (`HPFC_…`) under *About*.
3. Replace `HPFC_XXXXXXXXXXXX` in `HPFC.js` with that ID (add several IDs for several boards) and restart Zigbee2MQTT.

The device then shows up with `speed`, `temperature`, `humidity` and `filter_load`, also in Home Assistant through MQTT discovery.

## Known limitations of V0.1.4

- **Fan speed comes from Zigbee only.** The rotary encoder changes the value on the display, but the line that applies it to the fans (`SetFANSpeed(GetRotaryNum())` in the main loop) is commented out. A value set over Zigbee is not shown on the display.
- **Zigbee is required.** `ZigbeeInit()` waits until the HPFC has joined a network. With `ZIGBEE_EN 0` the fans can not be controlled until encoder control is enabled again.
- The fan RPM is measured and printed on the serial port but not yet reported or monitored.

Planned: power-on hours counter, calibration of the pressure difference, warning when a fan stops, interrupt-based button handling.

## License

[PolyForm Noncommercial 1.0.0](../LICENSE) – see [LICENSING.md](../LICENSING.md). Commercial use by agreement.
