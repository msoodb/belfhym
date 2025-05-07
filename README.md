# Belfhym 🕷️

**Belfhym** is a palm-sized autonomous flying machine designed for precision, stealth, and control. Powered by an embedded C flight controller, this ultra-lightweight micro air vehicle (MAV) fuses real-time responsiveness with minimalist engineering.

> *"Born in silence, Belfhym slips through the air with the mind of a machine and the soul of a ghost."*

---

## ✨ Features

- ✅ Real-time flight controller in C  
- ✅ Sensor fusion (MPU6050/ICM20948)  
- ✅ 3-axis PID stabilization (roll, pitch, yaw)  
- ✅ BLE or RF-based remote control interface  
- ✅ Modular and portable embedded code  
- ✅ Lightweight, power-optimized architecture  
- ✅ Failsafe logic and telemetry support  

---

## 📦 Hardware Requirements

| Component       | Recommendation / Example     |
|----------------|-------------------------------|
| MCU            | STM32F103, STM32L4, RP2040    |
| IMU            | MPU6050, ICM20948              |
| Motors         | 4× 6mm brushed coreless        |
| Power          | 1S LiPo 150–300mAh             |
| Radio          | NRF24L01+, BLE module          |
| Frame          | Carbon fiber / 3D-printed      |

---

## 🛠️ Getting Started

### 1. Clone the Repo

```bash
git clone https://github.com/yourname/belfhym.git
cd belfhym
make flash
```
