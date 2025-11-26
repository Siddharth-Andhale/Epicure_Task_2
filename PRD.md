# PRD – STM32F407VET6 UART + LED Timer System with MQTT Logging

## 1. Project Overview

This project implements a **non-blocking, fully interrupt-driven embedded system** on STM32F407VET6 with the following functionality:

- Transmit "IM ON\r\n" via UART every 1 second
- Toggle an LED via GPIO every 5 seconds
- UART output is received by an ESP and published to MQTT with timestamp
- Entire system must use **hardware timers only** (no delay, no blocking)

---

## 2. Objective

To design a deterministic timer-based communication and indication system with:

- Two independent hardware timers
- Non-blocking UART transmission
- MQTT data logging via ESP
- Clear verification of timing ratio (5:1)

---

## 3. Hardware Requirements

### 3.1 Microcontroller
- **STM32F407VET6**

### 3.2 External Module
- **ESP32 / ESP8266** (UART + WiFi)

---

## 4. Pin Configuration

| Function | STM32 Pin | Description |
|----------|-----------|-------------|
| UART TX  | PA9       | USART1 TX   |
| UART RX  | PA10      | USART1 RX   |
| LED      | PD12      | Onboard or External LED |
| ESP RX   | Connected to PA9 | Receives UART data from STM32 |
| ESP TX   | Connected to PA10 | Sends data to STM32 |

---

## 5. Functional Requirements

### 5.1 UART Transmission
- **Message to send:** `IM ON\r\n`
- **Frequency:** 1 second
- **Trigger:** Timer interrupt (TIM2)
- **No blocking UART calls**
- **Preferably:** `HAL_UART_Transmit_IT()` or DMA

### 5.2 LED Control
- **LED toggles state** every 5 seconds
- **Must be triggered** from a separate timer interrupt
- **GPIO pin:** PD12 is used for LED
- **No blocking GPIO calls**

### 5.3 ESP → MQTT Bridging
The ESP device must:
1. Receive UART messages from STM32
2. Add network time (via NTP)
3. Publish data to MQTT broker

#### MQTT Topic:
```
device/stm32/status
```

#### MQTT Payload Format:
```json
{
  "device": "STM32F407",
  "message": "IM ON",
  "timestamp": "2025-11-26T17:40:05Z"
}
```

---

## 6. Timer Configuration

| Timer | Peripheral | Use | Period |
|-------|-----------|-----|--------|
| TIM2  | 32-bit    | UART TX Timer | 1 second |
| TIM3  | 16-bit    | LED Timer | 5 seconds |

**All timers are configured in upcounting mode using internal clock.**

---

## 7. Software Design

### 7.1 Architecture Overview

```
┌──────────────────┐         ┌────────────────┐         ┌─────────────────┐
│ STM32F407VET6    │         │   ESP32/8266   │         │  MQTT Broker    │
│                  │─────────│  UART + WiFi   │────────│                 │
│ TIM2 (1s)        │ UART TX │   + MQTT       │ MQTT   │                 │
│ TIM3 (5s)        │         │                │        │                 │
└──────────────────┘         └────────────────┘         └─────────────────┘
```

### 7.2 Interrupt Flow

**TIM2 Interrupt (1s - UART Output)**
- Trigger UART transmit of "IM ON\r\n"

**TIM3 Interrupt (5s - LED Toggle)**
- Toggle LED GPIO state

---

## 8. Non-Functional Requirements

| Requirement | Description |
|-------------|-------------|
| **Non-blocking** | No delay-based code allowed |
| **Interrupt Driven** | All timing logic via ISR |
| **Deterministic** | Precise timing using hardware timers |
| **Low CPU Usage** | Minimal main loop utilization |

---

## 9. Software Architecture Details

### 9.1 Main Components

1. **Timer Initialization Module**
   - Configure TIM2 for 1-second period
   - Configure TIM3 for 5-second period
   - Setup interrupt priorities

2. **UART Module**
   - Initialize USART1 at appropriate baud rate
   - Configure for interrupt-driven TX
   - Use HAL_UART_Transmit_IT() or DMA

3. **GPIO Module**
   - Initialize PD12 as output
   - Configure for LED control

4. **Main Loop**
   - Minimal processing
   - Primarily sleeps/waits for interrupts

### 9.2 ISR Handlers

**TIM2_IRQHandler()**
```
- Call HAL_TIM_IRQHandler()
- Inside callback: Queue/trigger UART TX
```

**TIM3_IRQHandler()**
```
- Call HAL_TIM_IRQHandler()
- Inside callback: Toggle LED via HAL_GPIO_TogglePin()
```

---

## 10. Verification Criteria

- [ ] UART transmits "IM ON\r\n" exactly every 1 second
- [ ] LED toggles every 5 seconds (timing ratio 5:1 verified)
- [ ] No blocking code in main loop
- [ ] All interrupts trigger at precise intervals
- [ ] ESP receives UART data and publishes to MQTT with timestamp
- [ ] MQTT payload contains correct JSON format
- [ ] System operates indefinitely without drift

---

## 11. Tools & Dependencies

### STM32 Side
- **IDE:** STM32CubeIDE or PlatformIO
- **HAL:** STM32F4 HAL Library
- **Compiler:** ARM GCC

### ESP Side
- **IDE:** Arduino IDE or PlatformIO
- **Libraries:** 
  - `WiFi.h` (for WiFi connection)
  - `PubSubClient.h` (for MQTT)
  - `time.h` (for NTP)

---

## 12. Development Phases

### Phase 1: Timer & Interrupt Setup
- [ ] Configure TIM2 for 1-second interrupt
- [ ] Configure TIM3 for 5-second interrupt
- [ ] Verify interrupt triggering

### Phase 2: UART Implementation
- [ ] Initialize USART1
- [ ] Implement interrupt-driven UART TX
- [ ] Transmit "IM ON\r\n" in TIM2 ISR

### Phase 3: LED Control
- [ ] Initialize GPIO PD12
- [ ] Toggle LED in TIM3 ISR
- [ ] Verify 5-second toggle pattern

### Phase 4: ESP Integration
- [ ] Implement UART RX on ESP
- [ ] Parse received messages
- [ ] Add NTP time synchronization
- [ ] Publish to MQTT broker

### Phase 5: Testing & Verification
- [ ] Validate timing ratios
- [ ] Monitor MQTT messages
- [ ] Check for timing drift over extended periods

---

## 13. Testing Strategy

### Unit Tests
- Verify timer interrupt frequencies
- Validate UART message format
- Check GPIO toggle state

### Integration Tests
- Confirm end-to-end MQTT message delivery
- Validate JSON payload structure
- Verify timestamp accuracy

### System Tests
- Long-duration run (24+ hours) for drift detection
- MQTT broker connectivity handling
- WiFi reconnection scenarios (ESP)

---

## 14. Risks & Mitigation

| Risk | Impact | Mitigation |
|------|--------|-----------|
| Timer clock drift | Timing inaccuracy | Use stable internal/external clock; periodic NTP sync on ESP |
| UART buffer overflow | Message loss | Use DMA or buffering; monitor ESP RX capability |
| WiFi disconnection | MQTT failure | Implement reconnection logic on ESP |
| ISR race conditions | Data corruption | Use atomic operations; proper interrupt priorities |

---

## 15. Success Criteria

✓ System operates continuously without blocking  
✓ UART messages transmitted at precise 1-second intervals  
✓ LED toggle verified every 5 seconds (5:1 ratio confirmed)  
✓ MQTT broker receives timestamped JSON messages  
✓ Zero timing drift over 24-hour test period  
✓ All code is interrupt-driven, non-blocking  

---

**Document Version:** 1.0  
**Last Updated:** November 26, 2025  
**Status:** Ready for Development
