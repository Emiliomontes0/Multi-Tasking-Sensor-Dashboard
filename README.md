
# FreeRTOS Sensor Dashboard on ESP32-WROVER-CAM

This project implements a **real-time environmental sensor dashboard** using FreeRTOS on an **ESP32-WROVER-CAM (Freenove FNK0047B Kit)**. The system monitors ambient **light level (LDR)** and **temperature/humidity (DHT11)**, displaying live sensor data in a structured, easy-to-read **UART console dashboard.**

## Features
- **ESP32 FreeRTOS Multi-Tasking**
- **ADC Integration:** Light Level Sensor (LDR)
- **Digital Sensor Integration:** DHT11 Temperature & Humidity Sensor
- **FreeRTOS Queue-Based Communication** (Professional RTOS Design)
- **Dedicated Logger Task** for Dashboard Output
- **Formatted UART Dashboard with ANSI Clear Screen**
- **Real-Time Sensor Updates with Task Scheduling**

## 📂 Project Structure
```
main/
├── app_main.c        // Initializes queues and tasks
├── adc_task.c/.h     // LDR ADC readings, sends to queue
├── dht_task.c/.h     // DHT11 sensor, sends to queue
├── sensor_data.c/.h  // Queue management, logger task, dashboard
├── CMakeLists.txt    // ESP-IDF build config
```

## Architecture Overview

### **Queue-Based Communication**
- **Light Queue**: ADC task → Logger task
- **Temperature/Humidity Queue**: DHT11 task → Logger task
- **No Global Variables**: Professional RTOS design pattern

### **Task Responsibilities**
| Task | Priority | Responsibility |
|------|----------|----------------|
| `adc_task` | 5 | Reads LDR sensor, sends to light queue |
| `dht_task` | 5 | Reads DHT11 sensor, sends to temp/humidity queue |
| `logger_task` | 3 | Receives from queues, updates dashboard |

## Example UART Output (Simulated LCD):
```
---------------------------
[ Sensor Dashboard ]
Timestamp: 12345 ms
Light Level (ADC): 3410
Temperature: 24.5°C / 76.1°F
Humidity: 47.0%
---------------------------
```

## Hardware Used
| Component               | Purpose                |
|--------------------------|------------------------|
| **ESP32-WROVER-CAM (FNK0047B)** | Core MCU / FreeRTOS, UART |
| **LDR (Photoresistor)**  | Light Level Measurement (ADC GPIO32) |
| **DHT11 Sensor**         | Temperature & Humidity (GPIO25) |

## Software Stack
- **ESP-IDF v5.x+**
- **FreeRTOS (built-in with ESP-IDF)**
- **FreeRTOS Queues** for inter-task communication
- **ADC One-Shot Driver (ESP-IDF)**
- **GPIO Input Handling (ESP-IDF)**
- **UART0 Serial Output (USB to PC Terminal)**

## How It Works
| Task      | Interval | Action               |
|-----------|----------|-----------------------|
| `adc_task`| 1 second | Reads ADC (LDR), sends to light queue |
| `dht_task`| 3 seconds | Reads DHT11, sends to temp/humidity queue |
| `logger_task` | 100ms | Receives from queues, updates dashboard |

## Build & Flash
Ensure `idf.py` environment is set up properly.

```bash
idf.py build
idf.py -p /dev/cu.usbserial-110 flash monitor
```

## Skills Demonstrated
- **FreeRTOS Task Scheduling**
- **FreeRTOS Queue Communication**
- **Inter-task Data Sharing (Professional Pattern)**
- **UART Output Formatting**
- **ADC Peripheral Usage**
- **GPIO Input for Digital Sensors**
- **Real-Time Embedded Systems**
- **RTOS Architecture Design**