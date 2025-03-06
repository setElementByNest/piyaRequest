#include <ModbusRTUSlave.h>

#define RX_PIN 16
#define TX_PIN 17

#define MODBUS_BAUD 9600
#define MODBUS_CONFIG SERIAL_8N1
#define MODBUS_UNIT_ID 1

HardwareSerial RS485Serial(1);
ModbusRTUSlave modbus(RS485Serial);

uint16_t holdingRegisters[17]; //= {0, 0, 0, 0, 0, 0, 0};

void setup() {
  Serial.begin(115200);
  RS485Serial.begin(MODBUS_BAUD, MODBUS_CONFIG, RX_PIN, TX_PIN);

  modbus.configureHoldingRegisters(holdingRegisters, 17);

  modbus.begin(MODBUS_UNIT_ID, MODBUS_BAUD, MODBUS_CONFIG);

  Serial.println("Modbus RTU Slave Ready");
}

void loop() {
  String receive_names[6] = { "EC high", "pH high", "EC timer", "Fertilizer timer", "pH timer", "Acid timer" };
  static uint16_t holdingRegister = 0;
  holdingRegister++;

  // Assign values to registers
  holdingRegisters[0] = holdingRegister * 1;  // EC
  holdingRegisters[1] = holdingRegister * 2;  // PH
  holdingRegisters[2] = holdingRegister * 3;  // TEMP
  holdingRegisters[3] = holdingRegister * 4;  // PUY_A
  holdingRegisters[4] = holdingRegister * 5;  // PUY_B
  holdingRegisters[5] = holdingRegister * 6;  // PUY_C
  holdingRegisters[6] = holdingRegister * 7;  // ACID

  Serial.print("Slave: Holding Register = ");
  Serial.println(holdingRegister);

  // modbus.poll();

  if (modbus.poll()) {
    Serial.println("Write request received!");

    // Print the values of registers 11-16
    Serial.println("Updated Holding Registers (11-16):");
    for (int i = 11; i <= 16; i++) {
      // Serial.print("Register ");
      Serial.print(receive_names[i - 11]);
      Serial.print(": ");
      Serial.println(i != 12 ? holdingRegisters[i] : holdingRegisters[i] / 1000);
    }
  }

  delay(1000);
}
