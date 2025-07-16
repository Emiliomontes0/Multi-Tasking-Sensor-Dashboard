
# 📊 FreeRTOS Sensor Dashboard on ESP32-WROVER-CAM

This project implements a **real-time environmental sensor dashboard** using FreeRTOS on an **ESP32-WROVER-CAM (Freenove FNK0047B Kit)**. The system monitors ambient **light level (LDR)** and **temperature/humidity (DHT11)**, displaying live sensor data in a structured, easy-to-read **UART console dashboard.**

## 🛠️ Features
- **ESP32 FreeRTOS Multi-Tasking**
- **ADC Integration:** Light Level Sensor (LDR)
- **Digital Sensor Integration:** DHT11 Temperature & Humidity Sensor
- **Global Shared Data Structure for Sensor State**
- **Formatted UART Dashboard with ANSI Clear Screen**
- **Real-Time Sensor Updates with Task Scheduling**

## 📂 Project Structure
```
main/
├── app_main.c        // Initializes tasks
├── adc_task.c/.h     // LDR ADC readings, updates shared data
├── dht_task.c/.h     // DHT11 sensor, updates shared data
├── sensor_data.c/.h  // Shared global data, dashboard formatting
├── CMakeLists.txt    // ESP-IDF build config
```

## 📊 Example UART Output (Simulated LCD):
```
---------------------------
[ Sensor Dashboard ]
Light Level (ADC): 3410
Temperature: 24.5°C / 76.1°F
Humidity: 47.0%
---------------------------
```

## 🔧 Hardware Used
| Component               | Purpose                |
|--------------------------|------------------------|
| **ESP32-WROVER-CAM (FNK0047B)** | Core MCU / FreeRTOS, UART |
| **LDR (Photoresistor)**  | Light Level Measurement (ADC GPIO32) |
| **DHT11 Sensor**         | Temperature & Humidity (GPIO25) |

## ⚙️ Software Stack
- **ESP-IDF v5.x+**
- **FreeRTOS (built-in with ESP-IDF)**
- **ADC One-Shot Driver (ESP-IDF)**
- **GPIO Input Handling (ESP-IDF)**
- **UART0 Serial Output (USB to PC Terminal)**

## 🧑‍💻 How It Works
| Task      | Interval | Action               |
|-----------|----------|-----------------------|
| `adc_task`| 1 second | Reads ADC (LDR), updates dashboard |
| `dht_task`| 3 seconds | Reads DHT11, updates dashboard |
| `sensor_data` | Anytime | Holds current sensor state, clears screen, redraws |

## 🚀 Build & Flash
Ensure `idf.py` environment is set up properly.

```bash
idf.py build
idf.py -p /dev/cu.usbserial-110 flash monitor
```

## 📈 Skills Demonstrated
✅ FreeRTOS Task Scheduling  
✅ Inter-task Data Sharing  
✅ UART Output Formatting  
✅ ADC Peripheral Usage  
✅ GPIO Input for Digital Sensors  
✅ Real-Time Embedded Systems

## 📌 Potential Extensions
- BLE Environmental Beacon for Mobile Apps
- SD Card Logging (CSV)
- Wi-Fi Cloud Telemetry (MQTT / HTTP)
- Low-Power Optimization (Sleep Modes)
- FreeRTOS Queues / Notifications

## 📷 Screenshots
*(Optional — add a screenshot of your UART dashboard for visual impact)*
