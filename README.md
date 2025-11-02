# WiFi Geiger Counter Simulator

This hobby project simulates a Geiger-Müller counter's behavior but detects WiFi packets instead of radiation particles. It creates an audible "click" sound for each detected WiFi packet, similar to how a Geiger counter clicks when detecting radiation.

## Hardware Requirements

- ESP32 development board (most ESP32 boards are compatible)
- Passive piezo speaker
- 1kΩ resistor
- Jumper wires

## Wiring

```
ESP32 GPIO15 ----[1kΩ]---- Piezo(+)
ESP32 GND    ------------- Piezo(-)
```

The piezo speaker is connected to GPIO15 (chosen for its convenient location next to GND) through a 1kΩ current-limiting resistor.

## Features

- Detects WiFi packets and produces audible clicks
- Automatic channel hopping when no packets are detected
- Channel locking when active traffic is found
- Power-efficient operation
- No data storage or processing
- Minimal implementation focusing on packet presence detection only

## Privacy and Legal Considerations

**Important Notice regarding GDPR and Privacy Laws:**

This device is designed to comply with EU privacy regulations (GDPR) by implementing the following measures:
- Only detects the presence of WiFi packets without storing any data
- Does not collect, process, or store any personal information
- Does not decode packet contents
- Does not record MAC addresses or any network identifiers
- Does not perform any traffic analysis

**Usage Recommendations:**
- Use only on your own networks or with explicit permission
- Ideal for educational purposes and personal networking experiments
- Not intended for network analysis or monitoring of third-party networks
- Check local regulations before use

## Building and Flashing

This project uses PlatformIO for development. To build and flash:

1. Clone this repository
2. Open in PlatformIO
3. Build and upload to your ESP32

## Operation

When powered on, the device:
1. Scans WiFi channels (1-13)
2. Produces a click sound when packets are detected
3. Stays on active channels when continuous traffic is present
4. Resumes scanning after 1 second of inactivity

## License

This is free and unencumbered software released into the public domain. See the LICENSE file for details.

## Disclaimer

This is a hobby/educational project. Users are responsible for ensuring compliance with local laws and regulations regarding WiFi monitoring and privacy protection.