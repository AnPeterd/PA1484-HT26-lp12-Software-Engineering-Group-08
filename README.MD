#  THIS IS A TEMPLATE

> **This document is not final.** 
>
> Please fill in the sections relevant to your part
# Public Transport Information and Interaction on ESP32

Software Engineering project developed by Group 08 at Blekinge Institute of Technology.



## Overview

The system does... [describe what the system does]

## Requirements

The system requires an available Wi-Fi connection as well as 2 Trafiklab API keys:

- ResRobot v2.1 API key
- Trafiklab Realtime API key

Platform-IO is needed to compile and upload this software. 

## Dependencies

The project uses PlatformIO with the Arduino framework and Espressif32 platform version 6.7.0.

| Library | Configured Version |
|---|---|
| LVGL | 8.4.0 |
| ArduinoJson | 7.2.1 |
| TFT_eSPI | 2.5.0 |
| XPowersLib | 0.2.7 |
| SensorLib | 0.2.4 |
| AceButton | 1.10.1 |
| TinyGPSPlus | 1.0.3 |
| Adafruit NeoPixel | 1.11.0 |
| SparkFun MAX3010x | ^1.1.2 |
| OneWire | ^2.3.8 |

All dependencies are managed automatically by PlatformIO
according to `platformio.ini`.



## 	Installation and Setup

To run the program an ESP32-T4-S3 LilyGO is required. 
Additionally a secrets.h file with following code must be placed in the "project/" folder. 
```cpp
static const char* WIFI_SSID = "";
static const char* WIFI_PASSWORD = "";
static const char* API_KEY = "";
static const char* RESROBOT_API_KEY = "";
```
First two variables should contain Wi-Fi network name (SSID) and password, the last two corresponding API keys.

### Build

```bash
pio run
```

### Upload

Connect the device via USB, turn on the Upploading mode and run:

```bash
pio run -t upload
```

### Serial Monitor

```bash
pio device monitor
```


## Current Status

WIP (Work in progress)
For additional info please check the Scrum board project in the "Projects" tab.

## Hardware

The project runs on:

- **Microcontroller:** ESP32-T4-S3 LilyGO

## Technologies

- **Language:** C++ and C
- **Platform:** Espressif32 6.7.0
- **Framework:** Arduino
- **Build system:** PlatformIO
- **External APIs:** Trafiklab API
- **Version control:** Git and GitHub

## System Architecture

[Short explanation of the architecture.]

### The UI module:

### The Network module:

### The Input module:

### The Data Parsing module:
This module handles JSON data recieved from the Trafiklab API and stores the nessecary information in C++ objects, these objects are then used by the application to access the Trafiklab API data in a simple and effective way. 
It handles the incoming data by reading API streams with Arduino-Json, and later it creates a Departure or a Journey object - which holds that API data in the object variables, which are accessed through functions. The departure-parser validates required JSON fields, applies the selected transport filter, uses real-time departure information when available and sorts the departures by time. For the journey specific objects, those contain one or more JourneyLeg object representing parts of the trip, such as a walk, bus, train, or ferry ride. Each leg stores its origin, destination, departure time, arrival time, transport type, line number and direction - which allows the UI to easily display transfers and the complete route between two selected stops, by calling the journey objects. 


### The Settings storage module:

#### System Integration:


## Supported Locations / Data

The following stops are currently supported:

| Name | Identifier |
|---|---|
| Campus Gräsvik | 740032188 |
| Karlskrona Centralstation | 740000230 |
| Karlskrona centrum | 740023372 |
| Kungsplan | 740020510 |
| Bergåsa station | 740001537 |
| Karlskrona Amiralen | 740031800 |
| Torskors | 740011646 |
| NKT Arena Karlskrona | 740032192 |
| Lyckeby centrum | 740000663 |
| Ronneby resecentrum | 740000069 |


## Development Workflow

[Something here if needed]

## Testing

The project was tested using: 

## Team

| Member | Main Responsibility |
|---|---|
| Andrej | Network, API and partial integration |
| Loke | Parsing API data |
| Baqir | UI and partial integration |
| Samer | Input handling and event processing |
| Maksym | Settings storage and partial integration |


