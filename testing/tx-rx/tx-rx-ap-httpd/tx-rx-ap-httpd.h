#include <RadioLib.h>
#include "heltec.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

// LoRa Radio Configuration
SX1276 radio = new Module(18, 26, 14, 35);  // NSS, DIO0, RST, DIO1

// Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
String displayLines[4];  // Buffer for 4 lines of text

// Wi-Fi Configuration
const char* apSSID = "LoRaGateway";
const char* apPassword = "password123";

// Web Server Configuration
AsyncWebServer server(80);

// Configuration Management
const char* configFilePath = "/config.json";

// User Management
struct User {
  String username;
  String key;
};
std::vector<User> users;

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

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Failed to mount SPIFFS");
    return;
  }

  // Load configuration
  loadConfig();

  // Start Wi-Fi AP
  WiFi.softAP(apSSID, apPassword);
  updateDisplay("Wi-Fi AP", "SSID: " + String(apSSID));

  // Start Web Server
  setupWebServer();
  server.begin();
  updateDisplay("Web Server", "Started on port 80");

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

void loadConfig() {
  if (SPIFFS.exists(configFilePath)) {
    File file = SPIFFS.open(configFilePath, "r");
    if (file) {
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, file);
      if (!error) {
        // Load configuration from JSON
        apSSID = doc["apSSID"].as<const char*>();
        apPassword = doc["apPassword"].as<const char*>();
        // Load users
        JsonArray usersArray = doc["users"].as<JsonArray>();
        for (JsonObject userObj : usersArray) {
          User user;
          user.username = userObj["username"].as<String>();
          user.key = userObj["key"].as<String>();
          users.push_back(user);
        }
      }
      file.close();
    }
  }
}

void saveConfig() {
  DynamicJsonDocument doc(1024);
  doc["apSSID"] = apSSID;
  doc["apPassword"] = apPassword;
  JsonArray usersArray = doc.createNestedArray("users");
  for (const User& user : users) {
    JsonObject userObj = usersArray.createNestedObject();
    userObj["username"] = user.username;
    userObj["key"] = user.key;
  }
  File file = SPIFFS.open(configFilePath, "w");
  if (file) {
    serializeJson(doc, file);
    file.close();
  }
}

void setupWebServer() {
  // Serve static files from SPIFFS
  server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");

  // API Endpoints
  server.on("/api/send", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("message", true)) {
      String message = request->getParam("message", true)->value();
      sendMessage(message);
      request->send(200, "text/plain", "Message sent");
    } else {
      request->send(400, "text/plain", "Missing message parameter");
    }
  });

  server.on("/api/users", HTTP_GET, [](AsyncWebServerRequest *request){
    DynamicJsonDocument doc(1024);
    JsonArray usersArray = doc.createNestedArray("users");
    for (const User& user : users) {
      JsonObject userObj = usersArray.createNestedObject();
      userObj["username"] = user.username;
      userObj["key"] = user.key;
    }
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  server.on("/api/addUser", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("username", true) && request->hasParam("key", true)) {
      User user;
      user.username = request->getParam("username", true)->value();
      user.key = request->getParam("key", true)->value();
      users.push_back(user);
      saveConfig();
      request->send(200, "text/plain", "User added");
    } else {
      request->send(400, "text/plain", "Missing username or key parameter");
    }
  });

  // Handle 404
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Not found");
  });
}
