# LoRa Gateway with Web Interface

This project implements a LoRa communication gateway combined with a Wi-Fi Access Point and an asynchronous web server. It is designed to transmit and receive LoRa messages, manage configuration files, and provide a web interface for monitoring and user management.

---

## Features

### Core Functionalities
- **LoRa Communication**
  - Supports sending and receiving messages over LoRa.
  - Configurable parameters: frequency, bandwidth, spreading factor, coding rate, sync word, and TX power.
  - Non-blocking message reception with RSSI and SNR status.

- **Wi-Fi Access Point**
  - Creates a Wi-Fi network for remote interaction.
  - Default credentials: 
    - SSID: `LoRaGateway`
    - Password: `password123`.

- **Web Interface**
  - Asynchronous web server using SPIFFS to serve static files.
  - API endpoints for message sending, user management, and configuration.

- **Configuration Management**
  - Reads/writes configuration to a JSON file stored in SPIFFS.
  - Configuration includes Wi-Fi credentials and user management data.

### Hardware Integration
- **Heltec Display**
  - Displays system status, messages, and real-time telemetry (RSSI, SNR, uptime).
- **ESP32**
  - Handles Wi-Fi, web server, and SPIFFS file system.
- **SX1276 LoRa Module**
  - Manages LoRa communication using the RadioLib library.

---

## Hardware Requirements
- **Microcontroller**: Heltec Wi-Fi LoRa 32 (ESP32-based).
- **LoRa Module**: SX1276 or compatible module.
- **Display**: Integrated OLED display (128x64 pixels).

---

## Software Requirements
- **Arduino IDE**: Install the following libraries:
  - [RadioLib](https://github.com/jgromes/RadioLib)
  - [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
  - [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
  - [SPIFFS](https://github.com/espressif/arduino-esp32)
  - [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
  - Heltec ESP32 board definitions.

---

## Usage

### Initial Setup
1. **Upload the firmware** to the ESP32 using Arduino IDE.
2. **Connect to the Wi-Fi AP** using the default SSID and password.
3. **Access the web server** at `http://192.168.4.1`.

### API Endpoints
#### 1. **Send Message**
- **Endpoint**: `/api/send`
- **Method**: POST
- **Parameter**: `message` (string)
- **Description**: Sends a LoRa message.
- **Example**:
  ```bash
  curl -X POST http://192.168.4.1/api/send -d "message=HelloWorld"
  ```

#### 2. Get Users
- **Endpoint**: /api/users
- **Method**: GET
- **Description**: Retrieves the list of registered users.

#### 3. Add User
- **Endpoint**: /api/addUser
- **Method**: POST
- **Parameters**:
- **username**: User's name.
- **key**: User's key.
- **Description**: Adds a new user.

  ```bash
  curl -X POST http://192.168.4.1/api/addUser -d "username=John&key=1234"
  ```
  
### Configuration
#### Default Configuration File
**The configuration file (/config.json) is stored in SPIFFS and includes**:

  ```bash
  {
    "apSSID": "LoRaGateway",
    "apPassword": "password123",
    "users": [
      {"username": "admin", "key": "admin123"}
    ]
  }
  ```

## Functions Overview
**setup()**: Initializes all peripherals (LoRa, display, SPIFFS, Wi-Fi).
**loop()**: Handles LoRa communication and serial input.
**sendMessage(String message)**: Sends a LoRa message.
**receiveMessage()**: Receives LoRa messages and updates the display.
**updateDisplay(String header, String message)**: Updates the OLED display.
**loadConfig() and saveConfig()**: Manage JSON configuration.
**setupWebServer()**: Configures the asynchronous web server.
