
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// ==================================================
// 1. WIFI SETTINGS
// ==================================================
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ==================================================
// 2. MQTT SETTINGS
// ==================================================
// Enter the IP address of the computer running
// your MQTT broker (for example, Mosquitto).
const char* MQTT_SERVER = "192.168.1.100";
const int MQTT_PORT = 1883;

// Leave empty if your broker does not require login.
const char* MQTT_USER = "";
const char* MQTT_PASSWORD = "";

// ==================================================
// 3. ESP32 GPIO PIN ASSIGNMENTS
// ==================================================
const int LIGHT_PIN = 23;       // Relay/light control
const int FAN_INA_PIN = 25;    // Fan driver INA
const int FAN_INB_PIN = 26;    // Fan driver INB
const int WINDOW_PIN = 18;     // Window servo signal
const int DOOR_PIN = 19;       // Door servo signal
const int BUZZER_PIN = 21;     // Passive buzzer signal

// Change these if your relay is active-LOW.
const int RELAY_ON = HIGH;
const int RELAY_OFF = LOW;

// ==================================================
// 4. SERVO POSITIONS
// ==================================================
const int WINDOW_OPEN = 90;
const int WINDOW_CLOSED = 0;

const int DOOR_OPEN = 90;
const int DOOR_CLOSED = 0;

// ==================================================
// 5. MQTT AND SERVO OBJECTS
// ==================================================
WiFiClient espClient;
PubSubClient mqtt(espClient);

Servo windowServo;
Servo doorServo;

// ==================================================
// 6. FAN FUNCTIONS
// ==================================================
void fanOn() {
  // Run the L9110 fan in one direction.
  digitalWrite(FAN_INA_PIN, HIGH);
  digitalWrite(FAN_INB_PIN, LOW);
}

void fanOff() {
  // Both inputs LOW stop the fan.
  digitalWrite(FAN_INA_PIN, LOW);
  digitalWrite(FAN_INB_PIN, LOW);
}

// ==================================================
// 7. MQTT MESSAGE CALLBACK
// ==================================================
void mqttCallback(char* topic, byte* payload,
                  unsigned int length) {

  String message = "";

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  message.trim();
  message.toUpperCase();

  Serial.println("\n----------------------------");
  Serial.print("MQTT topic: ");
  Serial.println(topic);
  Serial.print("Received message: ");
  Serial.println(message);

  // ---------------- LIGHT ----------------
  if (strcmp(topic, "house/light") == 0) {

    if (message == "ON") {
      digitalWrite(LIGHT_PIN, RELAY_ON);
      Serial.println("Light switched ON");
    }
    else if (message == "OFF") {
      digitalWrite(LIGHT_PIN, RELAY_OFF);
      Serial.println("Light switched OFF");
    }
  }

  // ---------------- FAN ------------------
  else if (strcmp(topic, "house/fan") == 0) {

    if (message == "ON") {
      fanOn();
      Serial.println("Fan switched ON");
    }
    else if (message == "OFF") {
      fanOff();
      Serial.println("Fan switched OFF");
    }
  }

  // ---------------- WINDOW ---------------
  else if (strcmp(topic, "house/window") == 0) {

    if (message == "OPEN") {
      windowServo.write(WINDOW_OPEN);
      Serial.println("Window OPENED");
    }
    else if (message == "CLOSE") {
      windowServo.write(WINDOW_CLOSED);
      Serial.println("Window CLOSED");
    }
  }

  // ---------------- DOOR -----------------
  else if (strcmp(topic, "house/door") == 0) {

    if (message == "OPEN") {
      doorServo.write(DOOR_OPEN);
      Serial.println("Door OPENED");
    }
    else if (message == "CLOSE") {
      doorServo.write(DOOR_CLOSED);
      Serial.println("Door CLOSED");
    }
  }

  // ---------------- BUZZER ---------------
  else if (strcmp(topic, "house/buzzer") == 0) {

    if (message == "ON") {
      // Passive buzzer needs an oscillating signal.
      tone(BUZZER_PIN, 2000);
      Serial.println("Buzzer ON");
    }
    else if (message == "OFF") {
      noTone(BUZZER_PIN);
      digitalWrite(BUZZER_PIN, LOW);
      Serial.println("Buzzer OFF");
    }
  }

  else {
    Serial.println("Unknown MQTT topic");
  }
}

// ==================================================
// 8. CONNECT TO WIFI
// ==================================================
void connectWiFi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
}

// ==================================================
// 9. CONNECT TO MQTT BROKER
// ==================================================
void connectMQTT() {

  while (!mqtt.connected()) {

    Serial.print("Connecting to MQTT broker...");

    uint64_t chipId = ESP.getEfuseMac();

    String clientId = "ESP32-SmartHouse-";
    clientId += String((uint32_t)(chipId & 0xFFFFFFFF), HEX);

    bool connected;

    if (strlen(MQTT_USER) > 0) {
      connected = mqtt.connect(
        clientId.c_str(),
        MQTT_USER,
        MQTT_PASSWORD
      );
    }
    else {
      connected = mqtt.connect(clientId.c_str());
    }

    if (connected) {

      Serial.println("MQTT connected!");

      // Subscribe to all five Node-RED topics.
      mqtt.subscribe("house/light");
      mqtt.subscribe("house/fan");
      mqtt.subscribe("house/window");
      mqtt.subscribe("house/door");
      mqtt.subscribe("house/buzzer");

      Serial.println("Subscribed to all house topics.");
    }
    else {

      Serial.print("MQTT connection failed. State: ");
      Serial.println(mqtt.state());

      delay(3000);
    }
  }
}

// ==================================================
// 10. SETUP
// ==================================================
void setup() {

  Serial.begin(115200);

  // Configure light and fan outputs.
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(FAN_INA_PIN, OUTPUT);
  pinMode(FAN_INB_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Start with devices switched off.
  digitalWrite(LIGHT_PIN, RELAY_OFF);
  fanOff();
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);

  // Configure the window servo.
  windowServo.setPeriodHertz(50);
  windowServo.attach(WINDOW_PIN, 500, 2400);
  windowServo.write(WINDOW_CLOSED);

  // Configure the door servo.
  doorServo.setPeriodHertz(50);
  doorServo.attach(DOOR_PIN, 500, 2400);
  doorServo.write(DOOR_CLOSED);

  // Connect to WiFi and MQTT.
  connectWiFi();

  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(256);

  connectMQTT();

  Serial.println("\nSmart House is ready!");
}

// ==================================================
// 11. MAIN LOOP
// ==================================================
void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqtt.connected()) {
    connectMQTT();
  }

  // Keep receiving MQTT messages from Node-RED.
  mqtt.loop();
}

