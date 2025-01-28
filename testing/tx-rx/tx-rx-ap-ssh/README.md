## Overview
This project integrates LoRa communication with WiFi Access Point (AP) functionality and Secure Shell (SSH) server setup on an ESP32 using the RadioLib and ESPAsyncWebServer libraries. It also includes a display for real-time updates and supports encrypted messages across multiple virtual channels.

## Features

- **LoRa Communication:**
  - Operates on predefined frequency channels.
  - AES encryption for secure messaging.
  - Modular design for transmitting and receiving messages.

- **WiFi Access Point:**
  - Configurable SSID, password, and channel.
  - Displays AP IP address on the OLED screen.

- **SSH Server:**
  - Placeholder for adding SSH functionality (requires additional library).
  - Secure credentials for user authentication.

- **Web Server:**
  - Simple server providing status information.
  - Extendable for additional control and monitoring.

- **Channel Management:**
  - Switch between frequency channels and encryption keys via serial commands.

- **Display:**
  - Real-time updates for system status, LoRa transmission, and error messages.

## Requirements

### Hardware
- Heltec ESP32 LoRa board (or similar with SX1276 radio module).
- OLED display (128x64 resolution).
- LoRa antenna.
- Power supply.

### Libraries
- [RadioLib](https://github.com/jgromes/RadioLib): For LoRa communication.
- [AESLib](https://github.com/DavyLandman/AESLib): For AES encryption.
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer): For asynchronous web server.
- [Heltec ESP32](https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series): For OLED support.

## Installation

1. Install the required libraries in your Arduino IDE.
2. Connect the Heltec ESP32 board to your computer.
3. Upload the script `tx-rx-ap-ssh.h` to your ESP32 board.
4. Connect the LoRa antenna and power up the board.

## Usage

### Serial Commands
- **Send Message:** Enter the message in the Serial Monitor to transmit.
- **Change Channel and Key:** Use `C <freq> <key>` to switch channels and encryption keys.
  - Example: `C 2 3` switches to the 2nd frequency channel and 3rd encryption key.

### WiFi Access Point
- Connect to the AP using the credentials:
  - SSID: `LoRa_AP`
  - Password: `password123`
- Access the web server at the IP address displayed on the OLED screen.

### SSH Server
- SSH server setup is a placeholder. Use an SSH library like [ESP32SSHServer](https://github.com/) to implement.

## Code Structure

### Main Components
- **LoRa Initialization:**
  - Configures the frequency, bandwidth, spreading factor, and encryption.
  - Handles errors during setup.

- **Encryption/Decryption:**
  - Uses AES to secure transmitted and received messages.

- **WiFi AP Configuration:**
  - Starts a WiFi access point with specified credentials.

- **SSH Server:**
  - Placeholder for secure SSH functionality.

- **Web Server:**
  - Provides a basic interface for monitoring the device.

- **Display:**
  - Shows system status, errors, and message updates.

## Next Steps
1. **SSH Implementation:** Add a functional SSH server using an appropriate library.
2. **Testing:** Thoroughly test LoRa communication, encryption, and AP functionalities.
3. **Web Server Expansion:** Add interactive controls and status updates via the web interface.
4. **Enhancements:** Improve error handling, optimize memory usage, and expand features.

## Troubleshooting
- **LoRa Initialization Failure:** Ensure the radio module connections are correct and the antenna is attached.
- **WiFi AP Not Visible:** Check AP credentials and ensure no conflicts with nearby networks.
- **Encryption Errors:** Verify the keys and ensure message length does not exceed 16 bytes.

## Contributing
Feel free to fork and modify the code to suit your needs. Contributions are welcome to enhance functionality or improve efficiency.
