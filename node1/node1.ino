/*
   SMART FARM SOIL NODE (Slave)
   FIXED Polling Mode
   Addressed Communication Working
*/

#include <HardwareSerial.h>
#include <SPI.h>
#include <LoRa.h>

/* ---------------- RS485 Pins ---------------- */
#define RE 33
#define DE 32
HardwareSerial mod(1);

/* ---------------- LoRa Pins ---------------- */
#define ss   5
#define rst  14
#define dio0 2

/* ---------------- Addresses ---------------- */
byte GatewayNode = 0xFF;
byte SoilNode    = 0xBB;

byte msgCount = 0;

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

  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(5);

  mod.write(command, 8);
  mod.flush();

  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  unsigned long startTime = millis();

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
/* Send Soil Data Packet */
/* ===================================================== */
void sendSoilPacket()
{
  float moist = readSensor(query_moist) / 10.0;
  float temp  = readSensor(query_temp) / 10.0;
  float phVal = readSensor(query_ph) / 10.0;

  int n = readSensor(query_nitro);
  int p = readSensor(query_phos);
  int k = readSensor(query_pota);

  String payload = "DATA:SOIL;";
  payload += "M=" + String(moist) + ";";
  payload += "T=" + String(temp) + ";";
  payload += "PH=" + String(phVal) + ";";
  payload += "N=" + String(n) + ";";
  payload += "P=" + String(p) + ";";
  payload += "K=" + String(k);

  Serial.println("\n📤 Sending Soil Packet...");
  Serial.println(payload);

  LoRa.beginPacket();
  LoRa.write(GatewayNode);       // Destination = Gateway
  LoRa.write(SoilNode);          // Sender = Soil Node
  LoRa.write(msgCount);          // Msg ID
  LoRa.write(payload.length());  // Payload Length
  LoRa.print(payload);
  LoRa.endPacket();

  Serial.println("✅ Soil Packet Sent Successfully!");

  msgCount++;

  /* ✅ IMPORTANT: Back to RX Mode */
  LoRa.receive();
}

/* ===================================================== */
/* Setup */
/* ===================================================== */
void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n===============================");
  Serial.println("🌱 SOIL NODE STARTING...");
  Serial.println("===============================");

  /* RS485 Init */
  mod.begin(4800, SERIAL_8N1, 16, 17);

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  digitalWrite(RE, LOW);
  digitalWrite(DE, LOW);

  /* LoRa Init */
  Serial.println("📡 Initializing LoRa...");

  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) // Changed frequency to 433 MHz for compatibility with Gateway
  {
    Serial.println("❌ LoRa Init FAILED!");
    while (1);
  }

  LoRa.enableCrc();

  Serial.println("✅ LoRa Initialized!");
  Serial.println("✅ Soil Node Ready Waiting for Poll Requests...\n");

  /* Start Listening */
  LoRa.receive();
}

/* ===================================================== */
/* Loop */
/* ===================================================== */
void loop()
{
  int packetSize = LoRa.parsePacket();

  if (!packetSize) return;

  /* Read Header */
  int recipient = LoRa.read();
  byte sender   = LoRa.read();
  LoRa.read(); // msgId
  LoRa.read(); // length

  /* Read Payload */
  String incoming = "";

  while (LoRa.available())
    incoming += (char)LoRa.read();

  Serial.println("\n📩 Packet Received!");
  Serial.print("Recipient: 0x");
  Serial.println(recipient, HEX);

  Serial.print("Sender: 0x");
  Serial.println(sender, HEX);

  Serial.println("Payload: " + incoming);

  /* ✅ Accept only Gateway Poll */
  if (recipient != SoilNode && recipient != 0xFF) return;
  if (sender != GatewayNode) return;

  /* If Request */
  if (incoming == "REQ:SOIL")
  {
    Serial.println("✅ Valid Request Received from Gateway!");
    sendSoilPacket();
  }
}
