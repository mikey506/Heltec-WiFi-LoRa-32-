#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <FS.h>
#include <LittleFS.h>

// Constants for sensors and relays
#define DHTPIN 4
#define DHTTYPE DHT22
#define WATER_SENSOR_PIN 35

// 8-Channel Relay Module Pins
#define RELAY1_PIN 16
#define RELAY2_PIN 17
#define RELAY3_PIN 18
#define RELAY4_PIN 19
#define RELAY5_PIN 21
#define RELAY6_PIN 22
#define RELAY7_PIN 23
#define RELAY8_PIN 25

// 2-Channel High-Amperage Relay Module Pins
#define HIGH_RELAY1_PIN 26
#define HIGH_RELAY2_PIN 27

DHT dht(DHTPIN, DHTTYPE);

// Wi-Fi Credentials
const char* ssid = "ESP32_AP";
const char* password = "admin123";

// HTTP server
AsyncWebServer server(80);

// Global variables for settings
struct Settings {
  String adminUser;
  String adminPassword;
  float tempMin;
  float tempMax;
  float humidityMin;
  float humidityMax;
  int waterLevelThreshold;
  String relayAssignments[10]; // Fan, Pump, Light, etc.
  bool relayStates[10];
} settings;

// Function prototypes
void initWiFi();
void initWebServer();
void loadSettings();
void saveSettings();
String generateDashboard();

void setup() {
  Serial.begin(115200);

  // Initialize filesystem
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  // Load settings
  loadSettings();

  // Initialize Wi-Fi
  initWiFi();

  // Initialize DHT sensor
  dht.begin();

  // Initialize relays
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  pinMode(RELAY5_PIN, OUTPUT);
  pinMode(RELAY6_PIN, OUTPUT);
  pinMode(RELAY7_PIN, OUTPUT);
  pinMode(RELAY8_PIN, OUTPUT);
  pinMode(HIGH_RELAY1_PIN, OUTPUT);
  pinMode(HIGH_RELAY2_PIN, OUTPUT);

  // Start HTTP server
  initWebServer();
}

void loop() {
  // Read sensors periodically
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Relay control logic based on temperature and humidity
  for (int i = 0; i < 10; i++) {
    if (settings.relayAssignments[i] == "Fan" &&
        (temperature > settings.tempMax || humidity > settings.humidityMax)) {
      digitalWrite(relayPin(i), HIGH); // Turn ON fan relay
      settings.relayStates[i] = true;
    } else if (settings.relayAssignments[i] == "Fan") {
      digitalWrite(relayPin(i), LOW); // Turn OFF fan relay
      settings.relayStates[i] = false;
    }
  }

  // Water level warning
  int waterLevel = analogRead(WATER_SENSOR_PIN);
  if (waterLevel < settings.waterLevelThreshold) {
    Serial.println("Low water level detected!");
  }

  delay(2000); // Adjust delay as needed
}

// Get relay pin by index
int relayPin(int index) {
  switch (index) {
    case 0: return RELAY1_PIN;
    case 1: return RELAY2_PIN;
    case 2: return RELAY3_PIN;
    case 3: return RELAY4_PIN;
    case 4: return RELAY5_PIN;
    case 5: return RELAY6_PIN;
    case 6: return RELAY7_PIN;
    case 7: return RELAY8_PIN;
    case 8: return HIGH_RELAY1_PIN;
    case 9: return HIGH_RELAY2_PIN;
    default: return -1;
  }
}

// Initialize Wi-Fi as AP + Client
void initWiFi() {
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
}

// Start HTTP server and define routes
void initWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", generateDashboard());
  });

  server.on("/settings", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("tempMin", true)) {
      settings.tempMin = request->getParam("tempMin", true)->value().toFloat();
    }
    saveSettings();
    request->send(200, "application/json", "{\"message\":\"Settings saved\"}");
  });

  server.begin();
}

// Generate dashboard HTML
String generateDashboard() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Dashboard</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 0;
      background-color: #f4f4f4;
    }
    header {
      background: #0073e6;
      color: white;
      padding: 1rem;
      text-align: center;
    }
    h1, h2 {
      margin: 0;
    }
    .container {
      margin: 2rem;
      padding: 2rem;
      background: white;
      border-radius: 8px;
      box-shadow: 0px 4px 6px rgba(0, 0, 0, 0.1);
    }
    .status {
      display: flex;
      justify-content: space-between;
      padding: 1rem 0;
    }
    .status div {
      flex: 1;
      margin: 0 1rem;
      text-align: center;
      border: 1px solid #ccc;
      border-radius: 8px;
      background: #f9f9f9;
      padding: 1rem;
    }
    .status div p {
      margin: 0.5rem 0;
      font-size: 1.1rem;
    }
    .relays {
      margin-top: 2rem;
    }
    .relay-item {
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 1rem;
      border-bottom: 1px solid #ddd;
    }
    .relay-item:last-child {
      border-bottom: none;
    }
    .relay-item button {
      padding: 0.5rem 1rem;
      background: #0073e6;
      color: white;
      border: none;
      border-radius: 4px;
      cursor: pointer;
    }
    .relay-item button:disabled {
      background: #ccc;
    }
    footer {
      text-align: center;
      padding: 1rem;
      margin-top: 2rem;
      background: #0073e6;
      color: white;
    }
  </style>
</head>
<body>
  <header>
    <h1>ESP32 Environmental Control</h1>
    <h2>Dashboard</h2>
  </header>
  <div class="container">
    <section class="status">
      <div>
        <h3>Temperature</h3>
        <p><strong>)rawliteral" + String(dht.readTemperature()) + R"rawliteral( °C</strong></p>
      </div>
      <div>
        <h3>Humidity</h3>
        <p><strong>)rawliteral" + String(dht.readHumidity()) + R"rawliteral( %</strong></p>
      </div>
      <div>
        <h3>Water Level</h3>
        <p><strong>)rawliteral" + String(analogRead(WATER_SENSOR_PIN)) + R"rawliteral(</strong></p>
      </div>
    </section>
    <section class="relays">
      <h3>Relay Control</h3>
      <div>
)rawliteral";

  // Add relay controls dynamically
  for (int i = 0; i < 10; i++) {
    html += R"rawliteral(
        <div class="relay-item">
          <span>Relay )rawliteral" + String(i + 1) + R"rawliteral(: )rawliteral" + settings.relayAssignments[i] + R"rawliteral(</span>
          <button onclick="toggleRelay()rawliteral" + String(i) + R"rawliteral()">)rawliteral" +
            (settings.relayStates[i] ? "Turn OFF" : "Turn ON") +
            R"rawliteral(</button>
        </div>
    )rawliteral";
  }

  // Closing HTML
  html += R"rawliteral(
      </div>
    </section>
  </div>
  <footer>
    <p>&copy; 2025 HiLetgo ESP32 LoRa Environmental Control</p>
  </footer>
  <script>
    function toggleRelay(index) {
      fetch(`/toggle?relay=${index}`)
        .then(response => response.json())
        .then(data => {
          alert(data.message);
          location.reload();
        })
        .catch(error => console.error('Error:', error));
    }
  </script>
</body>
</html>
  )rawliteral";

  return html;
}


// Load settings from JSON file
void loadSettings() {
  if (LittleFS.exists("/settings.json")) {
    File file = LittleFS.open("/settings.json", "r");
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, file);
    file.close();

    settings.tempMin = doc["tempMin"];
    settings.tempMax = doc["tempMax"];
    settings.humidityMin = doc["humidityMin"];
    settings.humidityMax = doc["humidityMax"];
    settings.waterLevelThreshold = doc["waterLevelThreshold"];
    for (int i = 0; i < 10; i++) {
      settings.relayAssignments[i] = doc["relayAssignments"][i].as<String>();
      settings.relayStates[i] = doc["relayStates"][i];
    }
  }
}

// Save settings to JSON file
void saveSettings() {
  StaticJsonDocument<1024> doc;
  doc["tempMin"] = settings.tempMin;
  doc["tempMax"] = settings.tempMax;
  doc["humidityMin"] = settings.humidityMin;
  doc["humidityMax"] = settings.humidityMax;
  doc["waterLevelThreshold"] = settings.waterLevelThreshold;

  JsonArray relayAssignments = doc.createNestedArray("relayAssignments");
  JsonArray relayStates = doc.createNestedArray("relayStates");
  for (int i = 0; i < 10; i++) {
    relayAssignments.add(settings.relayAssignments[i]);
    relayStates.add(settings.relayStates[i]);
  }

  File file = LittleFS.open("/settings.json", "w");
  serializeJson(doc, file);
  file.close();
}
