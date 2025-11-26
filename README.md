# Epicure Task 2 - STM32F407VET6 UART + LED Timer System with MQTT Logging

A complete embedded systems project implementing a **non-blocking, fully interrupt-driven system** with UART communication, GPIO LED control, and MQTT cloud logging via ESP32.

## 📋 Project Overview

This project combines:
- **STM32F407VET6**: Microcontroller with hardware timer-based UART transmission and LED control
- **ESP32**: WiFi-enabled MQTT bridge for cloud data logging
- **HiveMQ Cloud**: MQTT broker for real-time data transmission

### Key Features
✅ Non-blocking UART transmission (1 message per second)  
✅ Interrupt-driven LED toggling (every 5 seconds)  
✅ NTP time synchronization for accurate timestamps  
✅ JSON-formatted MQTT payloads  
✅ TLS/SSL encrypted MQTT connection  
✅ Deterministic timing (5:1 ratio verification)  

---

## 📁 Project Structure

```
Epicure Task 2/
├── Timer Delay Task/                      # STM32CubeIDE Project
│   ├── Core/
│   │   ├── Inc/
│   │   │   ├── main.h
│   │   │   ├── stm32f4xx_it.h
│   │   │   └── stm32f4xx_hal_conf.h
│   │   └── Src/
│   │       ├── main.c                     # Timer & UART TX ISR
│   │       ├── stm32f4xx_it.c             # Interrupt handlers
│   │       └── system_stm32f4xx.c
│   ├── Drivers/
│   │   ├── CMSIS/
│   │   └── STM32F4xx_HAL_Driver/
│   ├── platformio.ini
│   └── STM32F407VETX_FLASH.ld
│
├── ESP UART Reciever and MQTT Pub/        # PlatformIO Project
│   ├── src/
│   │   └── main.cpp                       # UART RX → MQTT bridge
│   ├── include/
│   │   └── config.h                       # WiFi & MQTT credentials
│   ├── lib/
│   └── platformio.ini
│
├── PRD.md                                  # Product Requirements Document
└── .gitignore                              # Git ignore file
```

---

## 🔌 Hardware Requirements

### Microcontroller
- **STM32F407VET6** (ARM Cortex-M4, 84MHz, 192KB RAM, 512KB Flash)

### External Module
- **ESP32 DevKit** (WiFi + Bluetooth, 240MHz dual-core)

### Connections
| STM32 Pin | ESP32 Pin | Function |
|-----------|-----------|----------|
| PA9 (UART TX) | GPIO16 (RX) | UART Data (1 msg/sec) |
| PA10 (UART RX) | GPIO17 (TX) | (Optional feedback) |
| PD12 (GPIO) | - | LED (toggles every 5s) |
| GND | GND | Common Ground |

---

## ⚙️ Configuration

### STM32 Setup (STM32CubeIDE)
- **TIM2**: 32-bit timer, 1-second period, generates UART TX interrupt
- **TIM3**: 16-bit timer, 5-second period, generates LED toggle interrupt
- **USART1**: 115200 baud, interrupt-driven TX
- **System Clock**: 84 MHz (via HSI + PLL)

### ESP32 Setup (PlatformIO)
Edit `ESP UART Reciever and MQTT Pub/include/config.h`:

```cpp
// WiFi Configuration
#define WIFI_SSID "Your_Network"
#define WIFI_PASSWORD "Your_Password"

// MQTT Configuration (HiveMQ Cloud)
#define MQTT_BROKER "af31a5fa47f74486be18c7a448f04ae4.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883                    // TLS port
#define MQTT_USER "Your_Username"
#define MQTT_PASS "Your_Password"

// MQTT Topic
#define MQTT_TOPIC "device/stm32/status"
```

---

## 🚀 Getting Started

### Building STM32 Project
```bash
# Open in STM32CubeIDE
# Or build via command line:
cd "Timer Delay Task"
arm-none-eabi-gcc -c core/src/main.c ...
```

### Building ESP32 Project
```bash
cd "ESP UART Reciever and MQTT Pub"

# Build
platformio run -e esp32dev

# Upload
platformio run -e esp32dev --target upload

# Monitor
platformio device monitor -b 115200
```

---

## 📊 Data Flow

```
STM32F407VET6 (PA9 UART TX)
    ↓ "IM ON\r\n" every 1 second
    ↓
ESP32 (GPIO16 UART RX)
    ↓ Parse & add timestamp
    ↓ Create JSON payload
    ↓ Encrypt with TLS
    ↓
HiveMQ MQTT Broker (Port 8883)
    ↓
Subscribe to: device/stm32/status
    ↓
JSON Message:
{
  "device": "STM32F407",
  "message": "IM ON",
  "timestamp": "2025-11-26T14:32:20Z",
  "sequence": 1
}
```

---

## 📈 Expected Output (ESP32 Serial Monitor)

```
[ESP32] ========================================
[ESP32] ESP32 STM32 UART to MQTT Bridge v1.0
[ESP32] ========================================
[ESP32] Connecting to WiFi...
..........
[ESP32] WiFi connected!
[ESP32] IP: 192.168.1.150
[ESP32] UART ready on GPIO16(RX)/GPIO17(TX)
[ESP32] TLS certificate configured
[ESP32] Setup complete!
[ESP32] MQTT connected!
[ESP32] Message #1: {"device":"STM32F407","message":"IM ON","timestamp":"2025-11-26T14:32:20Z","sequence":1}
[ESP32] Message #2: {"device":"STM32F407","message":"IM ON","timestamp":"2025-11-26T14:32:21Z","sequence":2}
```

---

## 🔍 Verification

### Monitor MQTT Topic
```bash
# Using mosquitto_sub
mosquitto_sub -h af31a5fa47f74486be18c7a448f04ae4.s1.eu.hivemq.cloud \
  -u Epicure_Task_Broker \
  -P "Epicure#2025" \
  -p 8883 \
  --cafile ca.crt \
  -t device/stm32/status

# Or use MQTT Explorer GUI for easier visualization
```

### Verify Timing Ratio
- **LED** should toggle every 5 seconds (ON→OFF→ON)
- **UART/MQTT** messages arrive every 1 second
- **Expected ratio**: 5 UART messages per 1 LED toggle

---

## 🔒 Security Notes

- **TLS/SSL enabled** on port 8883 (not 1883)
- **Root CA certificate** included in config.h (ISRG Root X1)
- **Credentials required** for HiveMQ Cloud access
- **Never commit** sensitive credentials to public repos

---

## 📝 Technical Specifications

| Parameter | STM32 | ESP32 |
|-----------|-------|-------|
| UART Baud | 115200 | 115200 |
| TIM2 Period | 1 second | N/A |
| TIM3 Period | 5 seconds | N/A |
| WiFi Standard | N/A | 802.11 b/g/n |
| MQTT Version | N/A | 3.1.1 |
| TLS Version | N/A | 1.2 |
| NTP Sync | N/A | pool.ntp.org |

---

## 🐛 Troubleshooting

### ESP32 WiFi Issues
- Verify SSID and password in `config.h`
- Check WiFi signal strength
- Restart router if needed

### MQTT Connection Failed
- Verify HiveMQ credentials
- Check port 8883 is not blocked by firewall
- Ensure TLS certificate is valid
- Test with `mosquitto_pub` first

### UART Data Not Received
- Verify GPIO16/17 connections
- Check baud rate (115200)
- Use logic analyzer to verify signals
- Ensure common ground between boards

### Timing Issues
- Verify crystal/clock settings on STM32
- Use oscilloscope to measure actual frequencies
- Check for ISR conflicts

---

## 📚 Documentation

- **PRD.md**: Complete project requirements and specifications
- **Timer Delay Task/README**: STM32 specific documentation
- **ESP UART Reciever and MQTT Pub/README**: ESP32 specific documentation

---

## 📄 License

This project is provided as-is for educational and commercial use.

---

## 👥 Contributors

- **Project**: Epicure Robotics Task 2
- **Date**: November 2025
- **Platform**: STM32F407VET6 + ESP32

---

## 📞 Support

For issues, questions, or improvements, refer to:
- PRD.md for technical specifications
- Individual project README files
- Hardware datasheets (STM32F407, ESP32)

---

**Last Updated**: November 26, 2025
