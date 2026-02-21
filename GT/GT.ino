/************************************************************
   SMART FARM GATEWAY MASTER
   1 Minute Slot Polling for SOIL + COW Nodes
************************************************************/

#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <LoRa.h>
#include <time.h>

/* ---------------- WiFi Credentials ---------------- */
#define WIFI_SSID     "Boys Hostel"
#define WIFI_PASSWORD "DS1e@SP78"

/* ---------------- Firebase URL ---------------- */
#define FIREBASE_URL  "https://smart-farm-40c57-default-rtdb.asia-southeast1.firebasedatabase.app"

/* ---------------- LoRa Pins ---------------- */
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  2

#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23

/* ---------------- Addresses ---------------- */
byte GatewayNode = 0xFF;
byte SoilNode    = 0xBB;
byte CowNode     = 0xCC;

byte msgCount = 0;

/* ===================================================== */
/* TIMESTAMP FUNCTION */
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
/* SEND POLL REQUEST */
/* ===================================================== */
void sendPollRequest(byte nodeAddress, String requestPayload)
{
  Serial.println("\n📤 Sending Poll Request → Node: 0x" + String(nodeAddress, HEX));
  Serial.println("Payload: " + requestPayload);

  if (LoRa.beginPacket() == 0) {
    Serial.println("❌ Failed to start LoRa packet!");
    return;
  }

  LoRa.write(nodeAddress);              // Destination Node
  LoRa.write(GatewayNode);              // Sender = Gateway
  LoRa.write(msgCount++);               // Msg ID
  LoRa.write(requestPayload.length());  // Payload Length
  LoRa.print(requestPayload);

  if (LoRa.endPacket() == 1) {
    Serial.println("✅ Poll Request Sent Successfully!");
  } else {
    Serial.println("❌ Failed to send Poll Request!");
  }

  LoRa.receive(); // ✅ Back to RX mode
}

/* ===================================================== */
/* WAIT FOR NODE RESPONSE (1 MIN SLOT) */
/* ===================================================== */
bool waitForNodeResponse(byte expectedSender, String &receivedMsg)
{
  Serial.println("⏳ Waiting 60 Seconds Slot...");

  unsigned long start = millis();

  while (millis() - start < 60000)   // ✅ FULL 1 MIN SLOT
  {
    int packetSize = LoRa.parsePacket();

    if (packetSize)
    {
      int recipient = LoRa.read();
      byte sender   = LoRa.read();
      LoRa.read(); // msgId
      LoRa.read(); // length

      String msg = "";
      while (LoRa.available())
        msg += (char)LoRa.read();

      Serial.println("\n📩 Packet Received!");
      Serial.print("Recipient: 0x");
      Serial.println(recipient, HEX);
      Serial.print("Sender: 0x");
      Serial.println(sender, HEX);
      Serial.println("Payload: " + msg);

      // ✅ Correct sender + recipient check
      if (recipient == GatewayNode && sender == expectedSender)
      {
        receivedMsg = msg;
        return true;
      }
      else {
        Serial.println("❌ Packet recipient or sender mismatch!");
      }
    }
  }

  Serial.println("❌ Timeout! No reply in 1 minute slot.");
  return false;
}

/* ===================================================== */
/* FIREBASE UPLOAD */
/* ===================================================== */
void uploadFirebase(String path, String jsonPayload)
{
  HTTPClient http;
  String url = String(FIREBASE_URL) + path + ".json";

  Serial.println("\n📤 Uploading to Firebase...");
  Serial.println(jsonPayload);

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int code = http.PUT(jsonPayload);

  Serial.println("Firebase Response Code: " + String(code));

  http.end();
}

/* ===================================================== */
/* HANDLE SOIL PACKET */
/* ===================================================== */
void handleSoilPacket(String msg)
{
  float m, t, ph;
  int n, p, k;

  sscanf(msg.c_str(),
         "DATA:SOIL;M=%f;T=%f;PH=%f;N=%d;P=%d;K=%d",
         &m, &t, &ph, &n, &p, &k);

  String json = "{";
  json += "\"moisture\":" + String(m) + ",";
  json += "\"temperature\":" + String(t) + ",";
  json += "\"ph\":" + String(ph) + ",";
  json += "\"nitrogen\":" + String(n) + ",";
  json += "\"phosphorus\":" + String(p) + ",";
  json += "\"potassium\":" + String(k) + ",";
  json += "\"timestamp\":\"" + getTimestamp() + "\"}";

  uploadFirebase("/farms/farm-001/soil/latest", json);
}

/* ===================================================== */
/* HANDLE COW PACKET */
/* ===================================================== */
void handleCowPacket(String msg)
{
  float lat, lng, dist, temp;
  int infence, bpm;

  sscanf(msg.c_str(),
         "DATA:COW;LAT=%f;LNG=%f;DIST=%f;INFENCE=%d;BPM=%d;TEMP=%f",
         &lat, &lng, &dist, &infence, &bpm, &temp);

  String json = "{";
  json += "\"lat\":" + String(lat, 6) + ",";
  json += "\"lng\":" + String(lng, 6) + ",";
  json += "\"distance_m\":" + String(dist) + ",";
  json += "\"inFence\":" + String(infence) + ",";
  json += "\"bpm\":" + String(bpm) + ",";
  json += "\"bodyTemp\":" + String(temp) + ",";
  json += "\"timestamp\":\"" + getTimestamp() + "\"}";

  uploadFirebase("/farms/farm-001/cow/latest", json);
}

/* ===================================================== */
/* SETUP */
/* ===================================================== */
void setup()
{
  Serial.begin(115200);

  /* WiFi */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected!");

  /* Time Sync */
  configTime(0, 0, "pool.ntp.org");
  delay(2000);

  /* LoRa Init */
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) // Changed frequency to 433 MHz for India
  {
    Serial.println("❌ LoRa Init Failed!");
    while (1);
  }

  LoRa.enableCrc();
  LoRa.receive();

  Serial.println("✅ Gateway Ready!");
}

/* ===================================================== */
/* LOOP (1 MIN PER NODE SLOT) */
/* ===================================================== */
void loop()
{
  String msg = "";

  /*************** SOIL SLOT (1 MIN) *****************/
  Serial.println("\n=======================================");
  Serial.println("🚜 SOIL NODE SLOT START (60 Seconds)");
  Serial.println("=======================================");

  sendPollRequest(SoilNode, "REQ:SOIL");

  if (waitForNodeResponse(SoilNode, msg))
  {
    handleSoilPacket(msg);
  }

  /*************** COW SLOT (1 MIN) *****************/
  Serial.println("\n=======================================");
  Serial.println("🐄 COW NODE SLOT START (60 Seconds)");
  Serial.println("=======================================");

  sendPollRequest(CowNode, "REQ:COW");

  if (waitForNodeResponse(CowNode, msg))
  {
    handleCowPacket(msg);
  }

  Serial.println("\n✅ Full Cycle Complete. Restarting...\n");
}
