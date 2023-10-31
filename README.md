# ESP32 Thermostat

<table>
  <tr>
    <td><img src="https://github.com/alesus97/ESP32-Thermostat/blob/main/images/ProgettoCSD_Completo1000x1000.png" width="450"></td>
    <td><img src="https://github.com/alesus97/ESP32-Thermostat/blob/main/images/RisultatoFinale.jpg" width="400"></td>
  </tr>
</table>


## Introduction

This is an ESP32-based thermostat project designed to control heating or cooling systems in your home or office. It utilizes the capabilities of the ESP32 microcontroller to provide a versatile and customizable thermostat solution.

## Features

- **Temperature Control**: The thermostat can regulate the temperature within a specified range.
- TODO: **WiFi Connectivity**: Connects to your WiFi network for remote control and monitoring.
- TODO: **Web Interface**: Provides a user-friendly web interface for configuring settings.
- TODO: **OTA Updates**: Supports over-the-air (OTA) firmware updates for easy maintenance.

## Getting Started

### Hardware Requirements

- ESP32 Development Board
- Temperature Sensor (e.g., DS18B20)
- Relay Module (if controlling a heating or cooling system)
- Power Supply
- LCD Display 16x02 with 

### Installation

1. Clone this repository to your local machine:

```bash
git clone https://github.com/alesus97/ESP32-Thermostat.git
```

1. Open the project in your preferred IDE (e.g., Arduino IDE, PlatformIO).

2. Connect the hardware components as per the schematic provided in the docs directory.

3. Upload the sketch to your ESP32 board.

4. Open the Serial Monitor to view the IP address assigned to your ESP32.

5. Access the thermostat interface via a web browser using the IP address.

### Hardware connections
![ESP32 Thermostat](https://github.com/alesus97/ESP32-Thermostat/blob/main/images/CollegamentiHardware.png)

### Configuration
Connect the device to a power supply and wait for bootstrap session.
Configure temperature setpoints and according to your preferences, using provided buttons.

### Usage
The thermostat will now control the connected heating or cooling system based on the configured settings.

### Contributing
Feel free to contribute to this project by opening issues or pull requests. Your input is highly valued!

### License
This project is licensed under the MIT License.

### Contact
For questions or inquiries, please contact alesus97.
