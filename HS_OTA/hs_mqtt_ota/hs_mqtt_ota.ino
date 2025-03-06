#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <Update.h>

// WiFi credentials
const char* ssid = "nammonnnt";
const char* password = "1212312121";

// MQTT broker details
const char* mqtt_server = "192.168.83.27";
const int mqtt_port = 1883;
const char* mqtt_user = "naste_mqtt";
const char* mqtt_password = "1234";

const char* firmwareURL = "https://bfarm-api.vercel.app/sketch_jan30b.ino.bin";

WiFiClient espClient;
PubSubClient client(espClient);

// MQTT topics
const char* topic_pub = "nnt";
const char* topic_sub = "nnt";

void setup_wifi() {
  delay(10);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message received: ");
  Serial.println(message);

  if (message == "update") {
    performOTA();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void performOTA() {
  HTTPClient http;
  http.begin(firmwareURL);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    if (!Update.begin(contentLength)) {
      Serial.println("Not enough space for OTA");
      return;
    }

    Serial.println("Updating firmware...");
    size_t written = Update.writeStream(*stream);
    if (written == contentLength) {
      Serial.println("Firmware written successfully");
    } else {
      Serial.printf("Error: Written bytes (%d) do not match content length (%d)\n", written, contentLength);
    }

    if (Update.end()) {
      Serial.println("Update complete! Restarting...");
      ESP.restart();
    } else {
      Serial.printf("Update failed with error: %d\n", Update.getError());
    }
  } else {
    Serial.printf("HTTP error: %d\n", httpCode);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publish a message every 5 seconds
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    client.publish(topic_pub, "Hello from ESP32");
  }
}
