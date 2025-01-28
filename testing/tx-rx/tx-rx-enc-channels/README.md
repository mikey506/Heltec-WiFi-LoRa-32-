# README: LoRa with Encrypted Virtual Channels

This project implements a LoRa communication system with encrypted virtual channels, allowing users to effectively increase the number of communication channels by combining frequency separation and AES encryption keys. It uses the RadioLib library for LoRa radio communication and an AES encryption library for secure message encoding.

## Features

1. **LoRa Frequency Channels**:
   - Supports multiple physical frequency channels (e.g., 915.0, 915.125 MHz, etc.).
   - Configurable channel selection via Serial commands.

2. **Encrypted Virtual Channels**:
   - Adds AES encryption for each channel to differentiate groups using the same frequency.
   - Supports multiple 16-byte AES keys, one for each virtual channel.

3. **Dynamic Channel and Key Switching**:
   - Switch frequency and encryption key dynamically using Serial input (e.g., `C <freq> <key>`).

4. **OLED Display Integration**:
   - Displays the current frequency, key, and system status.
   - Shows transmission and reception logs in real-time.

5. **Error Handling**:
   - Handles invalid channel and key selections gracefully.
   - Provides feedback for transmission and reception status.

## Hardware Requirements

- Heltec LoRa Development Board (or equivalent with built-in OLED and SX1276/SX1278 LoRa chip).
- AES encryption library (lightweight library for embedded devices).

## Software Dependencies

- [RadioLib](https://github.com/jgromes/RadioLib): LoRa communication library.
- AES encryption library (add via platform-specific package manager).

## Getting Started

### Setup Instructions

1. Install the required libraries:
   - Install RadioLib via the Arduino Library Manager.
   - Add an AES encryption library to your project.

2. Open the `tx-rx-enc-channels.h` file in your preferred IDE (e.g., Arduino IDE or PlatformIO).

3. Configure the hardware:
   - Connect the Heltec board to your computer via USB.
   - Ensure the proper pin definitions for your LoRa module.

4. Upload the code to your Heltec board.

### Serial Commands

- To send a message:
  - Enter the text and press Enter.
  - The message will be encrypted and sent.

- To switch channels:
  - Use the command `C <freq> <key>` (e.g., `C 2 3`).
  - `freq` refers to the frequency channel index (1-based).
  - `key` refers to the encryption key index (1-based).

### Example Use Case

1. Start the system. The default channel is `1-1` (frequency channel 1 and key 1).
2. Send a message via the Serial Monitor (e.g., `Hello, Channel 1`).
3. Switch to channel `2-3` by entering `C 2 3`.
4. Send another message. Only devices on `2-3` can decrypt it.

## Code Highlights

### Channel Definition

```cpp
const float CHANNEL_FREQUENCIES[] = {915.0, 915.125, 915.25, 915.375};
const uint8_t CHANNEL_KEYS[][16] = {
    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F},
    {0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00}
};
```

### Encryption/Decryption

```cpp
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
```

## Future Enhancements

1. **Longer Message Support**:
   - Implement message chunking for encrypting longer texts.

2. **Authentication**:
   - Add a handshake protocol to validate devices.

3. **Dynamic Key Generation**:
   - Implement Diffie-Hellman or similar key exchange.

4. **GUI for Serial Interaction**:
   - Build a graphical interface to simplify channel and key management.

---

For any issues or feature requests, feel free to open a GitHub issue or contribute directly to the repository.
