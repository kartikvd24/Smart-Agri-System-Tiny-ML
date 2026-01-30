#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <LoRa.h>
#include <time.h>

/* ---------------- WiFi Credentials ---------------- */
#define WIFI_SSID "Boys Hostel"
#define WIFI_PASSWORD "DS1e@SP78"

/* ---------------- Firebase Database URL ---------------- */
#define FIREBASE_URL "https://smart-farm-40c57-default-rtdb.asia-southeast1.firebasedatabase.app"

/* ---------------- LoRa RX Pins (ESP32 + Ra-02) ---------------- */
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  2

/* SPI Pins (Fixed for ESP32) */
#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23

/* ---------------- Farm IDs ---------------- */
String FARM_ID = "farm-001";
String SOIL_NODE_ID = "soil-node-01";

/* ---------------- Sensor Variables ---------------- */
float moisture, temperature, ph;
int nitrogen, phosphorous, potassium;

/* ---------------- Backup Values ---------------- */
float last_moisture, last_temperature, last_ph;
int last_n, last_p, last_k;

/* ---------------- Timeout Handling ---------------- */
unsigned long lastPacketTime = 0;
const unsigned long LORA_TIMEOUT = 15000;
bool hasValidData = false;

/* ===================================================== */
/* Timestamp Function */
/* ===================================================== */
String getTimestamp()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
    return "time-error";

  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(buffer);
}

/* ===================================================== */
/* Parse LoRa Data */
/* ===================================================== */
void parseSensorData(String data)
{
  sscanf(data.c_str(),
         "Moisture:%f,Temperature:%f,pH:%f,N:%d,P:%d,K:%d",
         &moisture, &temperature, &ph,
         &nitrogen, &phosphorous, &potassium);
}

/* ===================================================== */
/* Upload Data to Firebase REST */
/* ===================================================== */
void sendToFirebase(float m, float t, float phVal,
                    int nVal, int pVal, int kVal)
{
  HTTPClient http;

  String url =
      String(FIREBASE_URL) +
      "/farms/" + FARM_ID +
      "/soilNodes/" + SOIL_NODE_ID +
      "/latest.json";

  String jsonPayload = "{";
  jsonPayload += "\"moisture\":" + String(m) + ",";
  jsonPayload += "\"temp_c\":" + String(t) + ",";
  jsonPayload += "\"ph\":" + String(phVal) + ",";
  jsonPayload += "\"n\":" + String(nVal) + ",";
  jsonPayload += "\"p\":" + String(pVal) + ",";
  jsonPayload += "\"k\":" + String(kVal) + ",";
  jsonPayload += "\"timestamp\":\"" + getTimestamp() + "\"";
  jsonPayload += "}";

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int code = http.PUT(jsonPayload);

  if (code > 0)
  {
    Serial.println("✅ Firebase Upload Success");
    Serial.println("HTTP Code: " + String(code));
  }
  else
  {
    Serial.println("❌ Firebase Upload Failed");
  }

  http.end();
}

/* ===================================================== */
/* Setup */
/* ===================================================== */
void setup()
{
  Serial.begin(115200);

  /* WiFi Connection */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n✅ WiFi Connected!");

  /* Time Sync */
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  /* Random Initial Values */
  randomSeed(analogRead(0));
  last_moisture = random(30, 90) / 100.0;
  last_temperature = random(200, 350) / 10.0;
  last_ph = random(55, 80) / 10.0;
  last_n = random(5, 40);
  last_p = random(5, 25);
  last_k = random(10, 60);

  hasValidData = true;

  /* Upload Initial Random Data */
  sendToFirebase(last_moisture, last_temperature, last_ph,
                 last_n, last_p, last_k);

  /* ---------------- LoRa Receiver Setup ---------------- */

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(865E6))   // India Band Recommended
  {
    Serial.println("❌ LoRa Init Failed!");
    while (1);
  }

  /* Best Long Range RX Settings */
  LoRa.setSpreadingFactor(12);     // SF12
  LoRa.setSignalBandwidth(125E3);  // BW 125kHz
  LoRa.setCodingRate4(8);          // CR 4/8
  LoRa.enableCrc();                // CRC ON

  Serial.println("✅ LoRa Receiver Started...");
}

/* ===================================================== */
/* Loop */
/* ===================================================== */
void loop()
{
  int packetSize = LoRa.parsePacket();

  /* CASE 1: Packet Received */
  if (packetSize)
  {
    String receivedData = "";

    while (LoRa.available())
      receivedData += (char)LoRa.read();

    Serial.println("\n📩 Packet Received:");
    Serial.println(receivedData);

    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());

    Serial.print("SNR: ");
    Serial.println(LoRa.packetSnr());

    parseSensorData(receivedData);

    /* Save Last Known Values */
    last_moisture = moisture;
    last_temperature = temperature;
    last_ph = ph;
    last_n = nitrogen;
    last_p = phosphorous;
    last_k = potassium;

    lastPacketTime = millis();

    /* Upload New Data */
    sendToFirebase(moisture, temperature, ph,
                   nitrogen, phosphorous, potassium);
  }

  /* CASE 2: Timeout Upload */
  if (hasValidData && (millis() - lastPacketTime > LORA_TIMEOUT))
  {
    Serial.println("\n⚠ No LoRa Packet → Uploading Old Data");

    sendToFirebase(last_moisture, last_temperature, last_ph,
                   last_n, last_p, last_k);

    lastPacketTime = millis();
  }
}
