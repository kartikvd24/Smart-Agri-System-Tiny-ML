#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <LoRa.h>
#include <time.h>

/* ---------------- WiFi Credentials ---------------- */
#define WIFI_SSID "Boys Hostel"
#define WIFI_PASSWORD "DS1e@SP78"

/* ---------------- Firebase URL ---------------- */
#define FIREBASE_URL "https://smart-farm-40c57-default-rtdb.asia-southeast1.firebasedatabase.app"

/* ---------------- LoRa Pins ---------------- */
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  2

#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23

/* ---------------- Farm ID ---------------- */
String FARM_ID = "farm-001";

/* ---------------- Variables ---------------- */
/* Soil */
float moisture, temperature, ph;
int nitrogen, phosphorous, potassium;

/* Milk */
float milkPh, milkTemp;

/* Cow */
float cowLat, cowLng, cowTemp;
int cowBpm;

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
/* Upload JSON to Firebase */
/* ===================================================== */
void uploadToFirebase(String path, String jsonPayload)
{
  HTTPClient http;

  String url = String(FIREBASE_URL) + path + ".json";

  Serial.println("\n📤 Uploading to Firebase...");
  Serial.println("Path: " + url);
  Serial.println("Payload: " + jsonPayload);

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int code = http.PUT(jsonPayload);

  if (code > 0)
  {
    Serial.println("✅ Upload Success!");
    Serial.println("HTTP Code: " + String(code));
  }
  else
  {
    Serial.println("❌ Upload Failed!");
  }

  http.end();
}

/* ===================================================== */
/* Parse + Upload Soil Packet */
/* DATA:SOIL;M=0.64;T=22.2;PH=6.3;N=12;P=9;K=35
/* ===================================================== */
void handleSoilPacket(String msg)
{
  sscanf(msg.c_str(),
         "DATA:SOIL;M=%f;T=%f;PH=%f;N=%d;P=%d;K=%d",
         &moisture, &temperature, &ph,
         &nitrogen, &phosphorous, &potassium);

  String json = "{";
  json += "\"moisture\":" + String(moisture) + ",";
  json += "\"temp_c\":" + String(temperature) + ",";
  json += "\"ph\":" + String(ph) + ",";
  json += "\"n\":" + String(nitrogen) + ",";
  json += "\"p\":" + String(phosphorous) + ",";
  json += "\"k\":" + String(potassium) + ",";
  json += "\"timestamp\":\"" + getTimestamp() + "\"";
  json += "}";

  uploadToFirebase("/farms/" + FARM_ID + "/soilNodes/soil-node-01/latest", json);
}

/* ===================================================== */
/* Parse + Upload Milk Packet */
/* DATA:MILK;PH=6.44;TEMP=4.80
/* ===================================================== */
void handleMilkPacket(String msg)
{
  sscanf(msg.c_str(),
         "DATA:MILK;PH=%f;TEMP=%f",
         &milkPh, &milkTemp);

  String json = "{";
  json += "\"milkPh\":" + String(milkPh) + ",";
  json += "\"milkTemp\":" + String(milkTemp) + ",";
  json += "\"timestamp\":\"" + getTimestamp() + "\"";
  json += "}";

  uploadToFirebase("/farms/" + FARM_ID + "/milk/latest", json);
}

/* ===================================================== */
/* Parse + Upload Cow Packet */
/* DATA:COW;LAT=12.91;LNG=77.56;BPM=78;TEMP=30.5
/* ===================================================== */
void handleCowPacket(String msg)
{
  sscanf(msg.c_str(),
         "DATA:COW;LAT=%f;LNG=%f;BPM=%d;TEMP=%f",
         &cowLat, &cowLng, &cowBpm, &cowTemp);

  String json = "{";
  json += "\"lat\":" + String(cowLat, 6) + ",";
  json += "\"lng\":" + String(cowLng, 6) + ",";
  json += "\"bpm\":" + String(cowBpm) + ",";
  json += "\"temp\":" + String(cowTemp) + ",";
  json += "\"timestamp\":\"" + getTimestamp() + "\"";
  json += "}";

  uploadToFirebase("/farms/" + FARM_ID + "/animals/cow-001/latest", json);
}

/* ===================================================== */
/* Setup */
/* ===================================================== */
void setup()
{
  Serial.begin(115200);

  /* WiFi */
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

  /* LoRa Init */
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(865E6))
  {
    Serial.println("❌ LoRa Init Failed!");
    while (1);
  }

  /* Long Range Settings */
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  LoRa.enableCrc();

  Serial.println("\n✅ Gateway Ready (RX Only Mode)");
  Serial.println("Waiting for packets from SOIL / MILK / COW...");
}

/* ===================================================== */
/* Loop (Always Listening) */
/* ===================================================== */
void loop()
{
  int packetSize = LoRa.parsePacket();

  if (packetSize)
  {
    String msg = "";

    while (LoRa.available())
      msg += (char)LoRa.read();

    Serial.println("\n📩 Packet Received:");
    Serial.println(">>> " + msg);

    /* Identify Node Type */
    if (msg.startsWith("DATA:SOIL"))
      handleSoilPacket(msg);

    else if (msg.startsWith("DATA:MILK"))
      handleMilkPacket(msg);

    else if (msg.startsWith("DATA:COW"))
      handleCowPacket(msg);

    else
      Serial.println("⚠ Unknown Packet Ignored!");
  }
}
