#include <RadioLib.h>
#include <Wire.h>
#include <SSD1306Wire.h>

// OLED Display Configuration
SSD1306Wire display(0x3c, 4, 15);  // Address, SDA, SCL pins for Heltec

// LoRa Radio Configuration
SX1276 radio = new Module(18, 26, 14, 35);  // NSS, DIO0, RST, DIO1

// Global Variables
String inputString = "";
bool stringComplete = false;

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  while (!Serial);  // Wait for serial port (for boards with native USB)

  // Initialize OLED
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(0, 0, "Initializing...");
  display.display();

  // Initialize LoRa
  float freq = 915.0;    // Frequency in MHz
  float bw = 125.0;      // Bandwidth (kHz)
  uint8_t sf = 7;        // Spreading factor
  uint8_t cr = 5;        // Coding rate
  uint8_t syncWord = 0x12;
  int8_t power = 17;     // TX power in dBm

  int state = radio.begin(freq, bw, sf, cr, syncWord, power);
  radio.setCRC(false);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("LoRa initialized!");
    display.clear();
    display.drawString(0, 0, "LoRa Ready!");
    display.display();
  } else {
    Serial.print("LoRa init failed: ");
    Serial.println(state);
    while (true);  // Halt on failure
  }

  // Configure Serial input handling
  Serial.setTimeout(50);
  Serial.println("Enter text to send (max 240 chars):");
}

void loop() {
  // Handle Serial Input
  if (Serial.available()) {
    inputString = Serial.readString();
    inputString.trim();
    if (inputString.length() > 0) {
      sendMessage(inputString);
      inputString = "";
    }
  }

  // Receive Messages
  receiveMessage();
}

void sendMessage(String message) {
  // Display on OLED
  display.clear();
  display.drawString(0, 0, "Sending:");
  display.drawString(0, 12, message);
  display.display();

  // Transmit message
  int state = radio.transmit(message);
  
  if (state == RADIOLIB_ERR_NONE) {
    Serial.print("Sent: ");
    Serial.println(message);
  } else {
    Serial.print("Send failed: ");
    Serial.println(state);
  }
}

void receiveMessage() {
  String receivedStr;
  int state = radio.receive(receivedStr, 0);  // Non-blocking receive

  if (state == RADIOLIB_ERR_NONE) {
    // Display on Serial
    Serial.print("Received: ");
    Serial.println(receivedStr);

    // Display on OLED
    display.clear();
    display.drawString(0, 0, "Received:");
    display.drawString(0, 12, receivedStr);
    display.display();
  }
  else if (state != RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.print("Receive error: ");
    Serial.println(state);
  }
}
