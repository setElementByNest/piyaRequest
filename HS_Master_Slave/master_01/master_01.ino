#include <ModbusMaster.h>

ModbusMaster master;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  master.begin(1, Serial2);
}

void loop() {
  uint8_t result;
  uint16_t data[7];
  String sensor_names[7] = { "EC", "pH", "Temp", "Fer.-A", "Fer.-B", "Fer.-C", "ACID" };

  result = master.readHoldingRegisters(0x0000, 7);

  if (result == master.ku8MBSuccess) {
    for (uint8_t i = 0; i < 7; i++) {
      Serial.print(sensor_names[i]);
      Serial.print("\t");
    }
    Serial.print("\n");
    for (uint8_t i = 0; i < 7; i++) {
      data[i] = master.getResponseBuffer(i);
      Serial.print(data[i]);
      Serial.print(i < 6 ? "\t" : "\n\n");
    }
  } else {
    Serial.print("Read error: 0x");
    Serial.println(result, HEX);
  }

  uint8_t result_sent;
  uint16_t values_send[6] = { 1600, 6.5 * 1000, 5, 7, 8, 9 };

  for (uint8_t i = 0; i < 6; i++) {
    master.setTransmitBuffer(i, values_send[i]);
  }

  result_sent = master.writeMultipleRegisters(11, 6);

  if (result_sent == 0) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.print("Error: ");
    Serial.println(result_sent, HEX);
  }

  delay(3000);
}
