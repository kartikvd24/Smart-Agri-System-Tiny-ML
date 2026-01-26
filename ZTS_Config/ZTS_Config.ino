#include <HardwareSerial.h>

#define RE 33
#define DE 32

HardwareSerial mod(1);  // UART1 (RX = 16, TX = 17)

// Default Slave Address = 0x01
// Baud Rate = 4800 (default from datasheet)

uint8_t query_moist[] = {0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0A};
uint8_t query_temp[]  = {0x01,0x03,0x00,0x01,0x00,0x01,0xD5,0xCA};
uint8_t query_ph[]    = {0x01,0x03,0x00,0x03,0x00,0x01,0x74,0x0A};

uint8_t query_nitro[] = {0x01,0x03,0x00,0x04,0x00,0x01,0xC5,0xCB};
uint8_t query_phos[]  = {0x01,0x03,0x00,0x05,0x00,0x01,0x94,0x0B};
uint8_t query_pota[]  = {0x01,0x03,0x00,0x06,0x00,0x01,0x64,0x0B};

void setup() {
  Serial.begin(9600);
  mod.begin(4800, SERIAL_8N1, 16, 17); // RX=16, TX=17

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  digitalWrite(RE, LOW);
  digitalWrite(DE, LOW);

  delay(300);
}

// ----------- FUNCTION TO SEND QUERY & READ RESPONSE --------------
int readSensor(uint8_t *command) {
  uint8_t buffer[8];
  uint8_t index = 0;
  unsigned long startTime;

  // Enable TX mode
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(5);

  mod.write(command, 8);
  mod.flush();

  // Enable RX mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  startTime = millis();
  while (millis() - startTime < 300) {
    if (mod.available()) {
      buffer[index++] = mod.read();
      if (index >= 7) break;
    }
  }

  if (index < 5) return -1;

  int value = buffer[3] * 256 + buffer[4];
  return value;
}

void loop() {

  Serial.println("----- SENSOR DATA -----");

  float moist = readSensor(query_moist) / 10.0;
  Serial.print("Moisture: "); Serial.print(moist); Serial.println(" %");

  float temp = readSensor(query_temp) / 10.0;
  Serial.print("Temperature: "); Serial.print(temp); Serial.println(" °C");

  float ph = readSensor(query_ph) / 10.0;
  Serial.print("pH: "); Serial.println(ph);

  int n = readSensor(query_nitro);
  Serial.print("Nitrogen (N): "); Serial.println(n);

  int p = readSensor(query_phos);
  Serial.print("Phosphorous (P): "); Serial.println(p);

  int k = readSensor(query_pota);
  Serial.print("Potassium (K): "); Serial.println(k);

  Serial.println("------------------------\n");

  delay(3000);
}