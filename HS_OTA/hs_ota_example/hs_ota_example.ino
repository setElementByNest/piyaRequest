#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "nammonnnt";
const char* password = "1212312121";
const char* firmwareURL = "https://bfarm-api.vercel.app/sketch_jan30b.ino.bin";

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
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected!");

  performOTA();
}

void loop() {
}
