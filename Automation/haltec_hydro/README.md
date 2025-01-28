![image](https://github.com/user-attachments/assets/867574a7-ca6d-4c23-b7bb-c6d0fbb73a91)
# Project Overview: HiLetgo ESP32 V3 LoRa Environmental Control
This project uses an ESP32 to manage an environmental control system, integrating sensors and relays for automated temperature, humidity, and water level monitoring and control. The system features a Wi-Fi dashboard for real-time monitoring and relay control, with configurable settings stored in JSON format.

## Modules Used
**HiLetgo ESP32 V3 LoRa**:
Central controller with Wi-Fi and LoRa communication capabilities.

**DHT22 (AM2302)**:
Measures temperature and humidity.

**MH-Z19C CO₂ Sensor**:
Measures carbon dioxide concentration.

**Gravity Analog Water Level Sensor**:
Monitors water level in the reservoir.

**Songle SRD-05VDC-SL-C (8-Channel Relay Module)**:
Controls standard devices such as fans and pumps.

**HiLetgo 2-Channel High-Amperage Relay Module**:
Handles higher current loads like powerful pumps or heaters.

# Pinout for HiLetgo ESP32 V3 LoRa Environmental Control

## **Modules and Pin Connections**

### **DHT22 (AM2302)**
| **Description** | **ESP32 Pin** |
|------------------|---------------|
| Data Pin         | GPIO 4        |

### **Gravity Analog Water Level Sensor**
| **Description** | **ESP32 Pin** |
|------------------|---------------|
| Analog Output    | GPIO 35       |

### **8-Channel Relay Module (Songle SRD-05VDC-SL-C)**
| **Relay**       | **ESP32 Pin** |
|------------------|---------------|
| Relay 1 Control | GPIO 16       |
| Relay 2 Control | GPIO 17       |
| Relay 3 Control | GPIO 18       |
| Relay 4 Control | GPIO 19       |
| Relay 5 Control | GPIO 21       |
| Relay 6 Control | GPIO 22       |
| Relay 7 Control | GPIO 23       |
| Relay 8 Control | GPIO 25       |

### **2-Channel High-Amperage Relay Module (HiLetgo)**
| **Relay**       | **ESP32 Pin** |
|------------------|---------------|
| High Relay 1    | GPIO 26       |
| High Relay 2    | GPIO 27       |

# Resistor Requirements for HiLetgo ESP32 V3 LoRa Environmental Control

## **Summary of Resistor Needs**

| **Module**                           | **Resistor Requirement**                                   |
|--------------------------------------|----------------------------------------------------------|
| **DHT22 (AM2302)**                   | **10kΩ pull-up resistor** on the data pin (GPIO 4).      |
| **Gravity Analog Water Level Sensor**| No resistor required.                                    |
| **MH-Z19C CO₂ Sensor**               | Check UART logic level: Use a **voltage divider** if TX outputs 5V. |
| **Songle SRD-05VDC-SL-C (8-Channel)**| **1kΩ resistors** in series with control pins if no onboard resistors. |
| **HiLetgo 2-Channel High-Amperage Relay**| Same as above: **1kΩ resistors** if required.            |

---

Ensure resistors are added as specified for reliable and safe operation.
