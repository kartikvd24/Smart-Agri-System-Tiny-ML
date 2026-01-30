#include <HardwareSerial.h>
#include <SPI.h>
#include <LoRa.h>

/* ---------------- RS485 Pins ---------------- */
#define RE 33
#define DE 32

HardwareSerial mod(1);  // UART1 (RX=16, TX=17)

/* ---------------- LoRa Pins (ESP32 + Ra-02) ---------------- */
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

/* ---------------- Read Sensor Function ---------------- */
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

  int value = buffer[3] * 256 + buffer[4];
  return value;
}

/* ---------------- Setup ---------------- */
void setup()
{
  Serial.begin(9600);

  /* RS485 Init */
  mod.begin(4800, SERIAL_8N1, 16, 17);

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  digitalWrite(RE, LOW);
  digitalWrite(DE, LOW);

  /* LoRa Init */
  SPI.begin(18, 19, 23, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(865E6))   // India Frequency
  {
    Serial.println("❌ LoRa Init Failed!");
    while (1);
  }

  /* Best Long Range Settings */
  LoRa.setTxPower(20);           // Max Power
  LoRa.setSpreadingFactor(12);   // SF12 = Long Range
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  LoRa.enableCrc();

  Serial.println("✅ Soil Node LoRa Transmitter Ready");
}

/* ---------------- Loop ---------------- */
void loop()
{
  float moist = readSensor(query_moist) / 10.0;
  float temp  = readSensor(query_temp) / 10.0;
  float phVal = readSensor(query_ph) / 10.0;

  int n = readSensor(query_nitro);
  int p = readSensor(query_phos);
  int k = readSensor(query_pota);

  /* Create LoRa Packet */
  String packet = "Moisture:" + String(moist) +
                  ",Temperature:" + String(temp) +
                  ",pH:" + String(phVal) +
                  ",N:" + String(n) +
                  ",P:" + String(p) +
                  ",K:" + String(k);

  Serial.println("Sending Packet:");
  Serial.println(packet);

  /* Send via LoRa */
  LoRa.beginPacket();
  LoRa.print(packet);
  LoRa.endPacket();

  delay(3000);
}
