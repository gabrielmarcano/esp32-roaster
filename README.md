<h1 align="center">ESP32 Roaster Project</h1>
<p align="center">
  A project to control a peanut, coffee & cocoa roaster with an ESP32
</p>
<p align="center">
  <a href="https://github.com/sindresorhus/awesome">
    <img alt="Awesome" src="https://cdn.rawgit.com/sindresorhus/awesome/d7305f38d29fed78fa85652e3a63e154dd8e8829/media/badge.svg">
  </a>
  <a href="https://github.com/gabrielmarcano/esp32-roaster/releases">
	<img alt="GitHub release" src="https://img.shields.io/github/v/release/gabrielmarcano/esp32-roaster">
  </a>
  <a href="https://platformio.org/">
	<img alt="Powered by PlatformIO" src="https://img.shields.io/badge/powered_by-PlatformIO-orange">
  </a>
</p>

## Contents

- [Summary](#summary)
- [Project structure](#project-structure)
- [Hardware](#hardware)
- [Software](#software)
- [Wiring](#wiring)

## Summary

All logic depends on the data given by the **Thermocouple** & **DHT22** sensors, and the selected mode in the **4 Position Rotary Switch**. It's intention is to control 3 motors, which will turn on or off based on the temperature that it reaches. 

When the temperature reaches 100ºC, 150ºC or 190ºC (depending on the mode) it feeds a relay that controls the first motor,
and also starts a timer that can be 12, 15 or 18 minutes which also depends on the mode.

When the timer stops, a buzzer starts making noise and also feeds the other 2 relays that controls the second & third motor.

There will also be two buttons, one will add +1min to the time (and start the timer if it's stopped), and the other will substract -1min to the time.

> Motors can only be stopped manually by either the security button or through the web interface.

### Modes

Position     | Name         | Temperature      | Time
-------------|--------------|------------------|-----------
1            | Peanut       | 100ºC            | 12m
2            | Coffee       | 150ºC            | 15m
3            | Cocoa        | 190ºC            | 18m


> The default state of the switch does not set a timer.

## Project structure

The project structure is as follows:

Resource                         | Description
-------------------------------- | ----------------------------------------------------------------------
[src/](src)                      | The [main.cpp](/src/main.cpp) file with the code to be uploaded to esp32
[data/](data)                    | Static files written directly to the SPI flash file storage (SPIFFS)
[lib/](lib)                      | All additional libraries. Core libraries are installed via PlatformIO or written in **lib_deps** using the [platformio.ini](platformio.ini) file
[platformio.ini](platformio.ini) | PlatformIO project configuration file
[build/](build)                  | Release bin files
[server/](server)                | [Express](https://expressjs.com/) server for debugging


## Hardware

- **ESP32-DEVKIT-V1**: ESP32 Microcontroller
- **I²C 16x2 LCD Display**
- **Type K thermocouple**: Thermoelectrical thermometer
- **MAX6675**: Type K thermocouple digital converter
- **DHT22**: Humidity sensor
- **4 Position Rotary Switch**: Mode selector
- **Passive Buzzer**
- **Motor (x3)**
- **Push button (x2)**: Time adder/substractor
- **Relay (x3)**
- **7805 / 7812**: Voltage regulators
- **Capacitors**
- **Resistors**

## Software

### Web server

The ESP32 also act as a server for controlling the motor states, showing the temperature and humidity with gauges, and showing the remaining time in the timer. The server uses [SSE](https://developer.mozilla.org/en-US/docs/Web/API/Server-sent_events) to update the values on the web.

The web interface can _read_ all values, and can only _write_ to the motor states values.

OTA updates are available thanks to [ElegantOTA](https://github.com/ayushsharma82/ElegantOTA).

Resource                         | Description
-------------------------------- | ----------------------------------------------------------------------
/events                          | Event Source with `readings`, `timer` & `states` events
/data                            | **GET** - Request to update the temperature & humidity readings, timer remaining time and motors states on the web interface
/motors                          | **POST** - Request to control the state of the motors throught the web interface
/update                          | Firmware & Filesystem OTA updates

### VPN

The ESP32 is connected to a VPN using [Husarnet](https://github.com/husarnet/husarnet-esp32). With this, the web interface can be accessed remotely, and it also enables simple remote OTA updates.

## Wiring

> **TODO**: Upload circuit, PCB design, 3D design, Gerber files & ESP32 pinout
