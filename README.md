# ESP32-Portable-WiFi-Analyzer

A small portable Wi-Fi analyzer built with an ESP32-S2 Mini and an OLED SSD1306 display.

The device scans nearby Wi-Fi networks and shows useful information such as SSID, signal strength, channel, and security type.

Created as an open-source project for Hack Club Stardance.

---

## Features 

- Scan nearby Wi-Fi networks
- Display signal strength (RSSI)
- Show Wi-Fi channel
- Detect security type
- OLED display interface
- Button-controlled network switching
- Automatic rescanning every 15 seconds

---

## Demo

(Add photos or video here)

Example:

![WiFi Analyzer](images/device.jpg)

---

## Hardware

Main version:

Component | Description

ESP32-S2 Mini | Main controller and Wi-Fi scanner
OLED SSD1306 128x64 | Display
Push button | Navigation

Compatible with other ESP32 boards with minor pin changes.

---

## Wiring

OLED (I2C):

OLED | ESP32-S2 Mini

VCC | 3.3V
GND | GND
SDA | GPIO 9
SCL | GPIO 12

Button:

Button | ESP32-S2 Mini

Pin 1 | GPIO 0
Pin 2 | GND

---

## Installation

### 1. Install Arduino IDE

Download Arduino IDE:
https://www.arduino.cc/

### 2. Install ESP32 support

Install the ESP32 board package in Arduino IDE.

### 3. Install libraries

Required libraries:

- Adafruit SSD1306
- Adafruit GFX Library

### 4. Upload code
