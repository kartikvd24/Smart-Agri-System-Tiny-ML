#include <HardwareSerial.h>
#include <SPI.h>
#include <LoRa.h>

/* ---------------- RS485 Pins ---------------- */
#define RE 33
#define DE 32

HardwareSerial mod(1);  // UART1 (RX=16, TX=17)

/* ---------------- LoRa Pins ---------------- */
#define LORA_SS   5
#define LORA_RST  14
#define LORA_DIO0 2

/* ---------------- Queries ---------------- */
uint8_t query_moist[] = {0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0A};
uint8_t query_temp[]  = {0x01,0x03,0x00,0x01,0x00,0x01,0xD5,0xCA};
uint8_t query_ph[]    = {0x01,0x03,0x00,0x03,0x00,0x01,0x74,0x0A};

uint8_t query_nitro[] = {0x01,0x03,0x00,0x04,0x00,0x01,0xC5,0xCB};
uint8_t query_phos[]  = {0x01,0x03,0x00,0x05,0x00,0x01,0x94,0x0B};
uint8_t query_pota[]  = {0x01,0x03,0x00,0x06,0x00,0x01,0x64,0x0B};

/* ===================================================== */
/* Read Sensor Function */
/* ===================================================== */
int readSensor(uint8_t *command)
{
  uint8_t buffer[8];
  uint8_t index = 0;
  unsigned long startTime;

  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(5);

  mod.write(command, 8);
  mod.flush();

  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  startTime = millis();
  while (millis() - startTime < 300)
  {
    if (mod.available())
    {
      buffer[index++] = mod.read();
      if (index >= 7) break;
    }
  }

  if (index < 5) return -1;

  return buffer[3] * 256 + buffer[4];
}

/* ===================================================== */
/* Send Soil Packet */
/* ===================================================== */
void sendSoilPacket()
{
  float moist = readSensor(query_moist) / 10.0;
  float temp  = readSensor(query_temp) / 10.0;
  float phVal = readSensor(query_ph) / 10.0;

  int n = readSensor(query_nitro);
  int p = readSensor(query_phos);
  int k = readSensor(query_pota);

  String packet = "DATA:SOIL;";
  packet += "M=" + String(moist) + ";";
  packet += "T=" + String(temp) + ";";
  packet += "PH=" + String(phVal) + ";";
  packet += "N=" + String(n) + ";";
  packet += "P=" + String(p) + ";";
  packet += "K=" + String(k);

  Serial.println("\n📤 Sending Soil Packet:");
  Serial.println(packet);

  LoRa.beginPacket();
  LoRa.print(packet);
  LoRa.endPacket();

  Serial.println("✅ Soil Data Sent!");
}

/* ===================================================== */
/* Setup */
/* ===================================================== */
void setup()
{
  Serial.begin(115200);

  mod.begin(4800, SERIAL_8N1, 16, 17);

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  digitalWrite(RE, LOW);
  digitalWrite(DE, LOW);

  SPI.begin(18, 19, 23, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(865E6))
  {
    Serial.println("❌ LoRa Init Failed!");
    while (1);
  }

  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  LoRa.enableCrc();

  Serial.println("✅ Soil Node Started (Periodic TX Mode)");
}

/* ===================================================== */
/* Loop */
/* ===================================================== */
void loop()
{
  sendSoilPacket();

  delay(30000); // ✅ Every 30 seconds
}
