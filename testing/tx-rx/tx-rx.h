#include <RadioLib.h>
#include "heltec.h"

// LoRa Radio Configuration
SX1276 radio = new Module(18, 26, 14, 35);  // NSS, DIO0, RST, DIO1

// Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
String displayLines[4];  // Buffer for 4 lines of text

void setup() {
  // Initialize Heltec hardware (Display, disable LoRa init, enable Serial)
  Heltec.begin(true, false, true);  // Display = true, LoRa = false, Serial = true

  // Custom display initialization
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
  updateDisplay("System Init", "Starting LoRa...");

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
    updateDisplay("LoRa Status", "Initialized!");
    Serial.println("LoRa initialized!");
  } else {
    updateDisplay("LoRa Error", String(state));
    Serial.print("LoRa init failed: ");
    Serial.println(state);
    while (true);  // Halt on failure
  }

  // Serial setup
  Serial.setTimeout(50);
  updateDisplay("System Ready", "Freq: " + String(freq) + "MHz");
  Serial.println("Enter text to send:");
}

void loop() {
  handleSerialInput();
  receiveMessage();
}

void handleSerialInput() {
  static String inputBuffer;
  
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        sendMessage(inputBuffer);
        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
    }
  }
}

void sendMessage(String message) {
  // Truncate to 240 characters (LoRa packet limit)
  message = message.substring(0, 240);
  
  // Display update
  updateDisplay("Transmitting", message);

  // Transmit message
  int state = radio.transmit(message);
  
  if (state == RADIOLIB_ERR_NONE) {
    updateDisplay("Tx Success", message);
    Serial.print("Sent: ");
    Serial.println(message);
  } else {
    updateDisplay("Tx Failed", String(state));
    Serial.print("Send failed: ");
    Serial.println(state);
  }
}

void receiveMessage() {
  static uint32_t lastUpdate = 0;
  String receivedStr;
  int state = radio.receive(receivedStr, 0);  // Non-blocking receive

  if (state == RADIOLIB_ERR_NONE) {
    // Display handling
    updateDisplay("Received", receivedStr);
    
    // Serial output
    Serial.print("Received: ");
    Serial.println(receivedStr);
    
    // Blink LED on reception
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (state != RADIOLIB_ERR_RX_TIMEOUT) {
    updateDisplay("Rx Error", String(state));
    Serial.print("Receive error: ");
    Serial.println(state);
  }

  // Update status line every 2 seconds
  if(millis() - lastUpdate > 2000) {
    updateStatusLine();
    lastUpdate = millis();
  }
}

void updateDisplay(String header, String message) {
  // Shift previous messages up
  for(int i = 3; i > 0; i--) {
    displayLines[i] = displayLines[i-1];
  }
  displayLines[0] = header + ": " + message;

  // Update physical display
  Heltec.display->clear();
  for(int i = 0; i < 4; i++) {
    Heltec.display->drawString(0, i*12, displayLines[i]);
  }
  Heltec.display->display();
}

void updateStatusLine() {
  // Keep bottom line for status info
  displayLines[3] = "RSSI:" + String(radio.getRSSI()) + 
                   " SNR:" + String(radio.getSNR()) + 
                   " " + String(millis()/1000) + "s";
  Heltec.display->drawString(0, 36, displayLines[3]);
  Heltec.display->display();
}
