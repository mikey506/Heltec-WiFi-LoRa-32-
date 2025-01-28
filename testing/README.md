# Heltec LoRa 32 Text Transceiver ([tx-rx.h](https://github.com/mikey506/Heltec-WiFi-LoRa-32-/blob/main/testing/tx-rx.h))

Basic text transmission system for Heltec WiFi LoRa 32 (V2/V3) boards using raw LoRa communication.

![image](https://github.com/user-attachments/assets/867574a7-ca6d-4c23-b7bb-c6d0fbb73a91)


## Features

- **Raw LoRa Text Transmission**
- ASCII character broadcast/reception
- OLED display integration (message history + real-time stats)
- RSSI/SNR monitoring
- Serial terminal interface
- Non-blocking operation
- Frequency/channel configuration

## Hardware Requirements

- Heltec WiFi LoRa 32 (V2/V3)
- LoRa antenna (868/915MHz)
- Micro-USB cable
- (Optional) LiPo battery

## Libraries Used

| Library | Purpose | Version |
|---------|---------|---------|
| [RadioLib](https://github.com/jgromes/RadioLib) | LoRa communication | >=5.6.0 |
| [Heltec ESP32](https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series) | OLED & Hardware Control | >=2.0.0 |
| SPI | LoRa module interface | Built-in |
| Wire | I2C (OLED) | Built-in |

## Installation

1. **Add Heltec Board Support**
   - Arduino IDE: `File > Preferences > Additional Boards Manager URLs`
   - Add URL: `https://resource.heltec.cn/download/package_heltec_esp32_index.json`

2. **Install Libraries**
   - Arduino Library Manager:
     - "RadioLib" by Jan Gromeš
     - "Heltec ESP32 Dev Boards" by Heltec

## Usage

1. **Upload Sketch**
   - Select board: `Heltec WiFi LoRa 32 (V2)`
   - Set COM port
   - Compile & upload

2. **Serial Interface (115200 baud)**
   ```shell
   > Enter text to send:
   Hello World  # Send messages via Serial input
   Received: Test Message  # Last received
   Tx Success: Hello World # Last transmission
   RSSI:-67 SNR:8.5 214s   # Real-time stats
   ```
## Code Overview
**Key Functions:**
   ```shell
sendMessage(): Handles message transmission
receiveMessage(): Non-blocking reception
updateDisplay(): OLED management
updateStatusLine(): Signal metrics
```
