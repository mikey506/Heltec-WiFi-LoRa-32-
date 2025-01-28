// Path: tx-rx.h

#include <RadioLib.h>
#include <AES.h>
#include "heltec.h"

// LoRa Radio Configuration
SX1276 radio = new Module(18, 26, 14, 35);  // NSS, DIO0, RST, DIO1

// Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
String displayLines[4];  // Buffer for 4 lines of text

// Channel Definitions
const float CHANNEL_FREQUENCIES[] = {915.0, 915.125, 915.25, 915.375}; // Define 4 base frequencies
const int NUM_FREQUENCY_CHANNELS = sizeof(CHANNEL_FREQUENCIES) / sizeof(CHANNEL_FREQUENCIES[0]);

// Encryption Keys for Virtual Channels (16-byte AES keys)
const uint8_t CHANNEL_KEYS[][16] = {
    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F},
    {0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00},
    {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0xA7, 0xB8, 0xC9, 0xDA, 0xEB, 0xFC, 0xAD, 0xBE, 0xCF, 0xD0},
    {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F, 0x7A, 0x8B, 0x9C, 0xAD, 0xBE, 0xCF, 0xD0, 0xE1, 0xF2, 0x03}
};
const int NUM_KEYS = sizeof(CHANNEL_KEYS) / sizeof(CHANNEL_KEYS[0]);

// State Variables
int currentFrequencyChannel = 0;
int currentKeyIndex = 0;

// Common LoRa Parameters
float bw = 125.0;      // Bandwidth (kHz)
uint8_t sf = 7;        // Spreading factor
uint8_t cr = 5;        // Coding rate
uint8_t syncWord = 0x12;
int8_t power = 17;     // TX power in dBm

AES aes;

// Helper to switch keys
uint8_t currentKey[16];

void setup() {
  Heltec.begin(true, false, true);  // Display = true, LoRa = false, Serial = true
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);

  // Initialize key
  memcpy(currentKey, CHANNEL_KEYS[currentKeyIndex], 16);

  updateDisplay("System Init", "Starting LoRa...");

  // Initialize LoRa with the first channel
  initializeLoRa();

  Serial.setTimeout(50);
  updateDisplay("System Ready", "Ch: " + String(currentFrequencyChannel + 1) + "-" + String(currentKeyIndex + 1));
  Serial.println("Enter text to send or change channel:");
  Serial.println("'C <freq> <key>' to switch (e.g., 'C 2 3').");
}

void loop() {
  handleSerialInput();
  receiveMessage();
}

void initializeLoRa() {
  float freq = CHANNEL_FREQUENCIES[currentFrequencyChannel];
  int state = radio.begin(freq, bw, sf, cr, syncWord, power);
  radio.setCRC(false);

  if (state == RADIOLIB_ERR_NONE) {
    updateDisplay("LoRa Status", "Ch: " + String(currentFrequencyChannel + 1) + "-" + String(currentKeyIndex + 1));
    Serial.println("LoRa initialized on channel " + String(currentFrequencyChannel + 1) + "-" + String(currentKeyIndex + 1));
  } else {
    updateDisplay("LoRa Error", String(state));
    Serial.print("LoRa init failed: ");
    Serial.println(state);
    while (true);  // Halt on failure
  }
}

void handleSerialInput() {
  static String inputBuffer;
  
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        if (inputBuffer.startsWith("C ")) {
          // Channel and key change command
          int freqChannel, keyIndex;
          sscanf(inputBuffer.c_str(), "C %d %d", &freqChannel, &keyIndex);
          if (freqChannel > 0 && freqChannel <= NUM_FREQUENCY_CHANNELS &&
              keyIndex > 0 && keyIndex <= NUM_KEYS) {
            currentFrequencyChannel = freqChannel - 1;
            currentKeyIndex = keyIndex - 1;
            memcpy(currentKey, CHANNEL_KEYS[currentKeyIndex], 16);
            initializeLoRa();
          } else {
            updateDisplay("Error", "Invalid Ch/Key");
            Serial.println("Invalid frequency or key. Use 'C <freq> <key>' (e.g., 'C 2 3').");
          }
        } else {
          // Treat as a message to send
          sendMessage(inputBuffer);
        }
        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
    }
  }
}

void encryptMessage(String &message) {
  byte plaintext[16] = {0};
  byte ciphertext[16] = {0};
  int len = message.length() > 16 ? 16 : message.length();
  memcpy(plaintext, message.c_str(), len);

  aes.do_aes_encrypt(plaintext, 16, ciphertext, currentKey, 128);

  message = "";
  for (int i = 0; i < 16; i++) {
    message += String(ciphertext[i], HEX);
  }
}

void decryptMessage(String &message) {
  byte ciphertext[16] = {0};
  byte plaintext[16] = {0};
  for (int i = 0; i < 16; i++) {
    ciphertext[i] = (byte)strtol(message.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
  }

  aes.do_aes_decrypt(ciphertext, 16, plaintext, currentKey, 128);

  message = String((char *)plaintext);
}

void sendMessage(String message) {
  encryptMessage(message);  // Encrypt before sending
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
  String receivedStr;
  int state = radio.receive(receivedStr, 0);  // Non-blocking receive

  if (state == RADIOLIB_ERR_NONE) {
    decryptMessage(receivedStr);  // Decrypt after receiving
    updateDisplay("Received", receivedStr);
    Serial.print("Received: ");
    Serial.println(receivedStr);
  }
}
