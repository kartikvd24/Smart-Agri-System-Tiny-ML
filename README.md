# 🌾 A TinyML-Driven LoRa Smart Agriculture System with Live Dashboard and Chatbot

<p align="center">

![ESP32](https://img.shields.io/badge/ESP32-Microcontroller-blue)
![TinyML](https://img.shields.io/badge/TinyML-Edge%20AI-green)
![LoRa](https://img.shields.io/badge/LoRa-Long%20Range-orange)
![Firebase](https://img.shields.io/badge/Firebase-Realtime%20Database-yellow)
![Edge Impulse](https://img.shields.io/badge/Edge%20Impulse-TinyML-red)
![Python](https://img.shields.io/badge/Python-Backend-blue)
![License](https://img.shields.io/badge/License-MIT-success)

</p>

---

# 📖 Overview

Agriculture is one of the most important sectors supporting global food production. Traditional farming methods often depend on manual observation for soil health monitoring, fertilizer application, irrigation scheduling, livestock management, and milk quality assessment. These approaches are time-consuming, labor-intensive, and may lead to inefficient resource utilization, reduced crop productivity, and increased operational costs.

This project presents a **TinyML-Driven LoRa Smart Agriculture System** that combines **Internet of Things (IoT), Tiny Machine Learning (TinyML), LoRa communication, Firebase Cloud, and an AI-powered Chatbot** to provide an intelligent precision farming solution. Multiple ESP32-based sensor nodes continuously collect environmental, soil, livestock, and milk quality data from different locations within the farm. The collected information is transmitted over long distances using **SX1278 LoRa modules** to a central ESP32 Gateway.

The gateway processes all received sensor data and performs **on-device TinyML inference** to predict soil health, fertilizer recommendations, and irrigation requirements without depending on cloud computation. The processed data is uploaded to **Firebase Realtime Database**, where it is visualized through a responsive web dashboard. Additionally, the system synchronizes selected data with Google Sheets, allowing an AI-powered chatbot to provide instant responses regarding soil condition, fertilizer suggestions, irrigation recommendations, livestock status, and milk quality.

This integrated architecture minimizes communication cost, reduces internet dependency, improves response time, and enables intelligent agricultural decision-making through low-power embedded AI devices.

---

# 🎯 Problem Statement

Modern agricultural practices face several challenges including:

- Manual soil analysis
- Improper fertilizer usage
- Excessive water consumption
- Lack of continuous livestock monitoring
- Milk quality degradation
- Limited internet availability in rural regions
- High cloud processing costs
- Delayed decision making

The objective of this project is to develop a smart agricultural monitoring system capable of collecting field data, performing TinyML inference locally, and providing real-time recommendations through LoRa communication and cloud visualization.

---

# 🚜 Project Features

✅ Real-Time Soil Monitoring

✅ Livestock Health Monitoring

✅ Milk Quality Monitoring

✅ TinyML-Based Fertilizer Recommendation

✅ Smart Irrigation Recommendation

✅ Long-Range LoRa Communication

✅ ESP32 Gateway

✅ Firebase Realtime Database

✅ Interactive Live Dashboard

✅ AI Chatbot Integration

✅ GPS Tracking and Geofencing

✅ Google Sheets Synchronization

✅ Low Power Embedded AI

✅ Edge Impulse TinyML Model

---

# 📷 Project Media

<p align="center">
  <img src="https://github.com/user-attachments/assets/c9e3e9d1-bfdc-4251-b90f-5e170f6d1baa" width="500">
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/2706fbc2-e570-4105-b366-21cac3b100dc" width="500">
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/361bd4ad-5d53-4b07-88e5-ffd53228949c" width="700">
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/07507fe9-4db6-4d83-84f0-5c7a770e9f33" width="450">
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/ba8e8268-c0f1-46a3-af91-3f4302fd182a" width="700">
</p>
The repository contains the complete implementation of the Smart Agriculture System including:

- Hardware Setup Images
- System Architecture
- Sensor Node Images
- Dashboard Screenshots
- Firebase Database Structure
- TinyML Model
- Dataset Files
- ESP32 Source Code
- Gateway Source Code
- Chatbot Source Code

> **Note:** Images and media files are included only for demonstrating the complete implementation of the Smart Agriculture System.

---

# 🏗️ System Architecture



```
Sensor Nodes
      │
      ▼
 LoRa Communication
      │
      ▼
 Gateway ESP32
      │
      ▼
 TinyML Prediction
      │
      ▼
 Firebase Realtime Database
      │
      ▼
 Live Dashboard
      │
      ▼
 Google Sheets
      │
      ▼
 AI Chatbot
```

---

## Architecture Description

### 🌱 Sensor Nodes

Multiple ESP32-based sensor nodes continuously monitor agricultural parameters including:

- Soil Moisture
- Soil pH
- Electrical Conductivity
- Temperature
- Humidity
- NPK Values
- Livestock Parameters
- Milk Quality

---

### 📡 LoRa Communication

Each node transmits sensor information to the Gateway using SX1278 LoRa modules.

Advantages include:

- Long Communication Range
- Low Power Consumption
- Reliable Data Transfer
- Suitable for Rural Farms

---

### 🌐 Gateway ESP32

The Gateway acts as the central processing unit.

Responsibilities include:

- Receiving LoRa packets
- Validating sensor data
- TinyML prediction
- Uploading data to Firebase
- Dashboard synchronization

---

### 🧠 TinyML Prediction

The trained Edge Impulse TinyML model predicts:

- Soil Health
- Fertilizer Recommendation
- Irrigation Recommendation

The inference is executed directly on the ESP32 without requiring cloud processing.

---

### ☁ Firebase Realtime Database

Stores:

- Raw Sensor Data
- TinyML Predictions
- GPS Location
- Milk Parameters
- Livestock Health Data

---

### 📊 Live Dashboard

Displays:

- Soil Monitoring
- Cow Monitoring
- GPS Location
- Milk Quality
- Fertilizer Recommendation
- Irrigation Recommendation

---

### 🤖 AI Chatbot

The chatbot retrieves processed agricultural data from Google Sheets and provides:

- Fertilizer Advice
- Irrigation Recommendation
- Soil Status
- Milk Quality
- Livestock Information

---

# ⚙️ Hardware Used

| Component | Description |
|-----------|-------------|
| ESP32 | Main Controller |
| SX1278 LoRa Module | Long Range Communication |
| DHT22 | Temperature & Humidity Monitoring |
| Soil Moisture Sensor | Soil Water Content |
| Soil pH Sensor | Soil Acidity Measurement |
| Electrical Conductivity Sensor | Soil Conductivity |
| NPK Sensor | Nitrogen, Phosphorus & Potassium |
| GPS Module | Livestock Tracking |
| Heart Rate Sensor | Animal Health Monitoring |
| MQ Gas Sensor | Milk Spoilage Detection |
| pH Sensor | Milk Quality Analysis |
| Relay Module | Pump Switching |
| Water Pump | Automatic Irrigation |
| Power Supply | System Power |

---

# 💻 Software Used

| Software | Purpose |
|-----------|---------|
| Arduino IDE | ESP32 Programming |
| Edge Impulse | TinyML Model Development |
| Firebase | Cloud Database |
| HTML | Dashboard |
| CSS | Dashboard UI |
| JavaScript | Dashboard Logic |
| Python | Chatbot Backend |
| Google Sheets | Data Synchronization |
| n8n | Workflow Automation |
| GitHub | Version Control |
| VS Code | Development Environment |

---

# 🔄 Complete Project Workflow

The Smart Agriculture System follows a distributed architecture where multiple ESP32-based sensor nodes continuously monitor environmental and agricultural parameters. These nodes communicate with a central gateway using LoRa technology. The gateway performs TinyML inference locally and uploads processed information to Firebase for visualization and chatbot interaction.

---

## 🌱 Step 1 — Soil Monitoring Node

The Soil Monitoring Node continuously measures important soil parameters required for crop growth and fertilizer recommendation.

### Parameters Collected

- 🌡 Soil Temperature
- 💧 Soil Moisture
- 🌱 Soil pH
- ⚡ Electrical Conductivity (EC)
- 🌿 Nitrogen (N)
- 🌾 Phosphorus (P)
- 🍃 Potassium (K)
- 💨 Atmospheric Temperature
- 💦 Humidity

These values are periodically collected using ESP32 and transmitted to the Gateway through LoRa.

---

## 🐄 Step 2 — Livestock Monitoring Node

The livestock monitoring subsystem tracks animal health and movement.

### Parameters Collected

- 📍 GPS Location
- ❤️ Heart Rate
- 🌡 Body Temperature
- 🚧 Geofence Status

The GPS location is continuously updated to Firebase, enabling live visualization on the dashboard.

---

## 🥛 Step 3 — Milk Quality Monitoring Node

Milk quality is monitored immediately after collection.

### Parameters Collected

- 🧪 Milk pH
- 🌡 Milk Temperature
- ⚡ Electrical Conductivity
- ☁ Gas Concentration

The system detects abnormal milk conditions and provides alerts for possible spoilage.

---

## 📡 Step 4 — LoRa Communication

All ESP32 sensor nodes transmit sensor information using SX1278 LoRa modules.

### Communication Process

```
Soil Node
      │
      ▼
Livestock Node
      │
      ▼
Milk Node
      │
      ▼
SX1278 LoRa
      │
      ▼
Gateway ESP32
```

### Advantages

- Long communication range
- Low power consumption
- Reliable communication
- No Wi-Fi dependency
- Suitable for remote farms

---

## 🌐 Step 5 — Gateway ESP32

The Gateway receives packets from all LoRa sensor nodes.

### Gateway Responsibilities

- Receive LoRa packets
- Decode sensor information
- Validate received data
- Execute TinyML model
- Upload processed data to Firebase
- Synchronize dashboard

---

## 🧠 Step 6 — TinyML Inference

The TinyML model is trained using Edge Impulse.

### Input Features

- Soil Moisture
- Soil pH
- Temperature
- Humidity
- Electrical Conductivity
- NPK Values

### TinyML Output

- Soil Health Status
- Fertilizer Recommendation
- Irrigation Recommendation

TinyML inference executes locally on the ESP32 Gateway without requiring cloud processing.

---

## ☁ Step 7 — Firebase Realtime Database

After TinyML inference, all processed information is uploaded to Firebase.

### Database Stores

- Soil Parameters
- Livestock Data
- Milk Quality
- GPS Coordinates
- TinyML Predictions
- Alerts

Firebase enables real-time synchronization across multiple devices.

---

## 📊 Step 8 — Live Dashboard

The dashboard continuously retrieves data from Firebase.

### Dashboard Modules

- 🌱 Soil Monitoring
- 🐄 Livestock Monitoring
- 🥛 Milk Quality
- 📍 GPS Tracking
- 💧 Irrigation Recommendation
- 🌾 Fertilizer Recommendation
- 📈 Historical Graphs

The dashboard updates automatically whenever Firebase data changes.

---

## 📄 Step 9 — Google Sheets Synchronization

Processed Firebase data is periodically synchronized to Google Sheets using automation.

### Stored Information

- Soil Parameters
- TinyML Predictions
- GPS Data
- Milk Parameters
- Livestock Health

Google Sheets acts as an intermediate source for chatbot queries.

---

## 🤖 Step 10 — AI Chatbot

The chatbot provides conversational access to agricultural information.

### Capabilities

- Soil Status
- Fertilizer Recommendation
- Irrigation Advice
- Livestock Status
- Milk Quality
- GPS Location

The chatbot retrieves processed information and provides user-friendly responses.

---

# 🧠 TinyML Workflow

```
Dataset Collection
        │
        ▼
Data Cleaning
        │
        ▼
Feature Engineering
        │
        ▼
Edge Impulse
        │
        ▼
Model Training
        │
        ▼
Model Validation
        │
        ▼
Model Optimization
        │
        ▼
Arduino Library Export
        │
        ▼
ESP32 Deployment
        │
        ▼
Real-Time Prediction
```

### TinyML Development Process

1. Dataset collection
2. Data preprocessing
3. Feature selection
4. Model training
5. Validation
6. Quantization
7. Arduino Library export
8. ESP32 deployment
9. Real-time inference

---

# 📡 LoRa Communication

The Smart Agriculture System uses SX1278 LoRa modules to provide long-distance communication between sensor nodes and the Gateway.

## Sender Nodes

- Soil Monitoring Node
- Livestock Monitoring Node
- Milk Monitoring Node

## Receiver

Gateway ESP32

## Packet Structure

```
Node ID

↓

Sensor Type

↓

Sensor Values

↓

Timestamp

↓

Checksum
```

### Communication Advantages

- Long Range
- Low Power
- Reliable Transmission
- Low Cost
- Suitable for Agricultural Applications

---

# ☁ Firebase Architecture

```
ESP32 Sensor Nodes
        │
        ▼
SX1278 LoRa
        │
        ▼
Gateway ESP32
        │
        ▼
TinyML Prediction
        │
        ▼
Firebase Realtime Database
        │
        ▼
Dashboard
        │
        ▼
Google Sheets
        │
        ▼
AI Chatbot
```

Firebase provides centralized storage and real-time synchronization for all agricultural data.

---

# 📊 Dashboard

The web dashboard provides real-time visualization of agricultural data.

### Dashboard Modules

- 🌱 Soil Monitoring
- 🐄 Livestock Monitoring
- 🥛 Milk Monitoring
- 📍 GPS Tracking
- 🤖 TinyML Prediction
- 📈 Historical Charts
- 💧 Irrigation Recommendation
- 🌾 Fertilizer Recommendation

### Dashboard Features

- Real-Time Updates
- Responsive Interface
- Firebase Integration
- Interactive Charts
- Google Maps Integration
- Live GPS Tracking

---

# 🤖 AI Chatbot

The AI-powered chatbot enables farmers to interact with agricultural data through natural language.

### Chatbot Features

- Fertilizer Recommendation
- Soil Health Information
- Irrigation Advice
- Livestock Health Status
- Milk Quality Information
- GPS Location Tracking
- Firebase Data Retrieval
- Google Sheets Integration

---

# 📈 TinyML Results

## Model Performance

| Metric | Value |
|---------|-------|
| Accuracy | 95%+ |
| Precision | High |
| Recall | High |
| F1 Score | High |
| Inference Time | Low |
| Memory Usage | Optimized |

---

# 🧪 Experimental Setup

The prototype was implemented using multiple ESP32 development boards connected with LoRa modules and agricultural sensors.

The system was tested under laboratory and simulated agricultural environments to validate:

- LoRa Communication
- TinyML Inference
- Firebase Synchronization
- Dashboard Updates
- Chatbot Responses

---

# 🌍 Applications

- 🌾 Precision Agriculture
- 💧 Smart Irrigation
- 🐄 Livestock Monitoring
- 🥛 Dairy Farm Automation
- 🌱 Soil Health Monitoring
- 🚜 Smart Farming
- 🌍 Sustainable Agriculture
- 📊 Agricultural Data Analytics

---

# 🚀 Future Scope

Future improvements include:

- 🌦 Weather Prediction Integration
- 🚁 Drone-Based Crop Monitoring
- 🛰 Satellite Image Analysis
- 📱 Mobile Application
- 🔋 Solar-Powered Sensor Nodes
- 🤖 Advanced Deep Learning Models
- 🌍 Multi-language AI Chatbot
- ☁ Cloud-Based Analytics
- 🌿 Disease Detection
- 🚜 Autonomous Farm Vehicles

---

# 👨‍💻 Contributors

- Preetam Basavaraj Sansuddi
- Kartik Vijayasinha Desai
- Hridik Thejus K
- MD Riyan

---




