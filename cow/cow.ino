/*
   SMART FARM COW NODE (Slave)
   Addressed Polling Mode + Geofence Data
*/

#include <SPI.h>
#include <LoRa.h>
#include <TinyGPSPlus.h>
#include <math.h>

/* ---------------- LoRa Pins ---------------- */
#define ss   5
#define rst  14
#define dio0 2

/* ---------------- Addresses ---------------- */
byte GatewayNode = 0xFF;
byte CowNode     = 0xCC;

byte msgCount = 0;

/* ---------------- GPS ---------------- */
TinyGPSPlus gps;
HardwareSerial SerialGPS(2); // RX=16 TX=17

/* ---------------- Geofence Center ---------------- */
const float fence_lat = 12.911305;
const float fence_lon = 77.566978;
const float radius_km = 0.06; // 60 meters

/* ===================================================== */
/* Distance Calculation */
/* ===================================================== */
double calculateDistance(float lat1, float lon1, float lat2, float lon2)
{
  double R = 6371.0;

  double dLat = radians(lat2 - lat1);
  double dLon = radians(lon2 - lon1);

  double a =
      sin(dLat / 2) * sin(dLat / 2) +
      cos(radians(lat1)) * cos(radians(lat2)) *
      sin(dLon / 2) * sin(dLon / 2);

  double c = 2 * atan2(sqrt(a), sqrt(1 - a));

  return R * c;
}

/* ===================================================== */
/* Send Cow Packet */
/* ===================================================== */
void sendCowPacket()
{
  if (!gps.location.isValid())
  {
    Serial.println("❌ GPS Not Locked Yet!");
    return;
  }

  float lat = gps.location.lat();
  float lon = gps.location.lng();

  double dist_km = calculateDistance(fence_lat, fence_lon, lat, lon);
  double dist_m  = dist_km * 1000;

  int inFence = (dist_km <= radius_km) ? 1 : 0;

  int bpm = random(60, 95);
  float temp = random(280, 390) / 10.0;

  /* Payload */
  String payload = "DATA:COW;";
  payload += "LAT=" + String(lat, 6) + ";";
  payload += "LNG=" + String(lon, 6) + ";";
  payload += "DIST=" + String(dist_m, 1) + ";";
  payload += "INFENCE=" + String(inFence) + ";";
  payload += "BPM=" + String(bpm) + ";";
  payload += "TEMP=" + String(temp);

  Serial.println("\n📤 Sending Cow Packet...");
  Serial.println(payload);

  /* Send Packet with Header */
  LoRa.beginPacket();
  LoRa.write(GatewayNode);        // Destination
  LoRa.write(CowNode);            // Sender
  LoRa.write(msgCount);           // Msg ID
  LoRa.write(payload.length());   // Length
  LoRa.print(payload);
  LoRa.endPacket();

  msgCount++;
}

/* ===================================================== */
/* Setup */
/* ===================================================== */
void setup()
{
  Serial.begin(115200);

  /* GPS Init */
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);

  /* LoRa Init */
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) // Changed frequency to 433 MHz for compatibility with Gateway
  {
    Serial.println("❌ LoRa Init Failed!");
    while (1);
  }

  LoRa.enableCrc();
  Serial.println("✅ Cow Node Ready (Polling Mode)");

  randomSeed(analogRead(0));
}

/* ===================================================== */
/* Loop */
/* ===================================================== */
void loop()
{
  /* Keep GPS Updating */
  while (SerialGPS.available())
    gps.encode(SerialGPS.read());

  /* Wait for Gateway Request */
  int packetSize = LoRa.parsePacket();
  if (!packetSize) return;

  int recipient = LoRa.read();
  byte sender   = LoRa.read();
  LoRa.read(); // msgId
  LoRa.read(); // length

  String incoming = "";
  while (LoRa.available())
    incoming += (char)LoRa.read();

  if (recipient != CowNode) return;

  if (incoming == "REQ:COW")
  {
    Serial.println("\n📩 Request Received from Gateway!");
    sendCowPacket();
  }
}
