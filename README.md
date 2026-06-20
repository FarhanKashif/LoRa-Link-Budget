**📡 LoRa Link Budget Analyzer (SX127x)**

A real-time RF analysis tool built on ESP32 that reads low-level SX1278 LoRa registers and computes key link budget parameters such as RSSI, SNR, Noise Floor, Receiver Sensitivity, Path Loss, Link Margin, and Estimated Distance.

🚀 Overview

This project implements a **custom SX1278 driver** performs **real-time RF link analysis** using fundamental wireless communication principles.

It is designed to bridge the gap between:

📶 RF Engineering 
💻 Embedded Systems 

## ⚙️ Features

📡 Direct register-level communication with SX1278
  📊 Real-time calculation of:

  * RSSI (Received Signal Strength Indicator)
  * SNR (Signal-to-Noise Ratio)
  * Noise Floor
  * Receiver Sensitivity
  * Link Margin
  * Path Loss

## 🧠 Technical Insights

* RSSI values are approximations and may have ±5–7 dB error
* Near-field measurements (< ~3m) produce unreliable path loss results
* Real-world environments introduce additional losses (multipath, obstacles, antenna mismatch)


## 🤝 Contributions

Feel free to fork and improve.
