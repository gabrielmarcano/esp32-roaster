<h1 align="center">ESP32 Roaster Project</h1>
<p align="center">
  A project to control a peanut, coffee & cocoa roaster with an ESP32
</p>

<div align="center">

[![Build](https://img.shields.io/github/actions/workflow/status/gabrielmarcano/esp32-roaster/build.yml?logo=github)](https://github.com/gabrielmarcano/esp32-roaster/blob/master/.github/workflows/build.yml)
[![OTA Update](https://img.shields.io/github/actions/workflow/status/gabrielmarcano/esp32-roaster/ota-update.yml?logo=github&label=OTA)](https://github.com/gabrielmarcano/esp32-roaster/blob/master/.github/workflows/ota-update.yml)
[![GitHub release](https://img.shields.io/github/v/release/gabrielmarcano/esp32-roaster?filter=*alpha&logo=github)](https://github.com/gabrielmarcano/esp32-roaster/releases)
[![Build with PlatformIO](https://img.shields.io/badge/build%20with-PlatformIO-orange?logo=data%3Aimage%2Fsvg%2Bxml%3Bbase64%2CPHN2ZyB3aWR0aD0iMjUwMCIgaGVpZ2h0PSIyNTAwIiB2aWV3Qm94PSIwIDAgMjU2IDI1NiIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIiBwcmVzZXJ2ZUFzcGVjdFJhdGlvPSJ4TWlkWU1pZCI+PHBhdGggZD0iTTEyOCAwQzkzLjgxIDAgNjEuNjY2IDEzLjMxNCAzNy40OSAzNy40OSAxMy4zMTQgNjEuNjY2IDAgOTMuODEgMCAxMjhjMCAzNC4xOSAxMy4zMTQgNjYuMzM0IDM3LjQ5IDkwLjUxQzYxLjY2NiAyNDIuNjg2IDkzLjgxIDI1NiAxMjggMjU2YzM0LjE5IDAgNjYuMzM0LTEzLjMxNCA5MC41MS0zNy40OUMyNDIuNjg2IDE5NC4zMzQgMjU2IDE2Mi4xOSAyNTYgMTI4YzAtMzQuMTktMTMuMzE0LTY2LjMzNC0zNy40OS05MC41MUMxOTQuMzM0IDEzLjMxNCAxNjIuMTkgMCAxMjggMCIgZmlsbD0iI0ZGN0YwMCIvPjxwYXRoIGQ9Ik0yNDkuMzg2IDEyOGMwIDY3LjA0LTU0LjM0NyAxMjEuMzg2LTEyMS4zODYgMTIxLjM4NkM2MC45NiAyNDkuMzg2IDYuNjEzIDE5NS4wNCA2LjYxMyAxMjggNi42MTMgNjAuOTYgNjAuOTYgNi42MTQgMTI4IDYuNjE0YzY3LjA0IDAgMTIxLjM4NiA1NC4zNDYgMTIxLjM4NiAxMjEuMzg2IiBmaWxsPSIjRkZGIi8+PHBhdGggZD0iTTE2MC44NjkgNzQuMDYybDUuMTQ1LTE4LjUzN2M1LjI2NC0uNDcgOS4zOTItNC44ODYgOS4zOTItMTAuMjczIDAtNS43LTQuNjItMTAuMzItMTAuMzItMTAuMzJzLTEwLjMyIDQuNjItMTAuMzIgMTAuMzJjMCAzLjc1NSAyLjAxMyA3LjAzIDUuMDEgOC44MzdsLTUuMDUgMTguMTk1Yy0xNC40MzctMy42Ny0yNi42MjUtMy4zOS0yNi42MjUtMy4zOWwtMi4yNTggMS4wMXYxNDAuODcybDIuMjU4Ljc1M2MxMy42MTQgMCA3My4xNzctNDEuMTMzIDczLjMyMy04NS4yNyAwLTMxLjYyNC0yMS4wMjMtNDUuODI1LTQwLjU1NS01Mi4xOTd6TTE0Ni41MyAxNjQuOGMtMTEuNjE3LTE4LjU1Ny02LjcwNi02MS43NTEgMjMuNjQzLTY3LjkyNSA4LjMyLTEuMzMzIDE4LjUwOSA0LjEzNCAyMS41MSAxNi4yNzkgNy41ODIgMjUuNzY2LTM3LjAxNSA2MS44NDUtNDUuMTUzIDUxLjY0NnptMTguMjE2LTM5Ljc1MmE5LjM5OSA5LjM5OSAwIDAgMC05LjM5OSA5LjM5OSA5LjM5OSA5LjM5OSAwIDAgMCA5LjQgOS4zOTkgOS4zOTkgOS4zOTkgMCAwIDAgOS4zOTgtOS40IDkuMzk5IDkuMzk5IDAgMCAwLTkuMzk5LTkuMzk4em0yLjgxIDguNjcyYTIuMzc0IDIuMzc0IDAgMSAxIDAtNC43NDkgMi4zNzQgMi4zNzQgMCAwIDEgMCA0Ljc0OXoiIGZpbGw9IiNFNTcyMDAiLz48cGF0aCBkPSJNMTAxLjM3MSA3Mi43MDlsLTUuMDIzLTE4LjkwMWMyLjg3NC0xLjgzMiA0Ljc4Ni01LjA0IDQuNzg2LTguNzAxIDAtNS43LTQuNjItMTAuMzItMTAuMzItMTAuMzItNS42OTkgMC0xMC4zMTkgNC42Mi0xMC4zMTkgMTAuMzIgMCA1LjY4MiA0LjU5MiAxMC4yODkgMTAuMjY3IDEwLjMxN0w5NS44IDc0LjM3OGMtMTkuNjA5IDYuNTEtNDAuODg1IDIwLjc0Mi00MC44ODUgNTEuODguNDM2IDQ1LjAxIDU5LjU3MiA4NS4yNjcgNzMuMTg2IDg1LjI2N1Y2OC44OTJzLTEyLjI1Mi0uMDYyLTI2LjcyOSAzLjgxN3ptMTAuMzk1IDkyLjA5Yy04LjEzOCAxMC4yLTUyLjczNS0yNS44OC00NS4xNTQtNTEuNjQ1IDMuMDAyLTEyLjE0NSAxMy4xOS0xNy42MTIgMjEuNTExLTE2LjI4IDMwLjM1IDYuMTc1IDM1LjI2IDQ5LjM2OSAyMy42NDMgNjcuOTI2em0tMTguODItMzkuNDZhOS4zOTkgOS4zOTkgMCAwIDAtOS4zOTkgOS4zOTggOS4zOTkgOS4zOTkgMCAwIDAgOS40IDkuNCA5LjM5OSA5LjM5OSAwIDAgMCA5LjM5OC05LjQgOS4zOTkgOS4zOTkgMCAwIDAtOS4zOTktOS4zOTl6bS0yLjgxIDguNjcxYTIuMzc0IDIuMzc0IDAgMSAxIDAtNC43NDggMi4zNzQgMi4zNzQgMCAwIDEgMCA0Ljc0OHoiIGZpbGw9IiNGRjdGMDAiLz48L3N2Zz4=)](https://platformio.org/)

</div>

## Contents

- [Summary](#summary)
- [Project structure](#project-structure)
- [Hardware](#hardware)
- [Software](#software)
- [Wiring](#wiring)

## Summary

All logic depends on the data given by the **Thermocouple** & **DHT22** sensors, and the selected mode in the **4 Position Rotary Switch**. It's intention is to control 3 motors, which will turn on or off based on the temperature that it reaches.

When the temperature reaches 140ºC, 170ºC or 180ºC (depending on the mode) it feeds a relay that controls the first motor,
and also starts a timer that can be 12, 20 or 33 minutes which also depends on the mode.

There will be two push buttons, one will add +1min to the time (and start the timer if there isn't one already), and the other will reduce -1min to the time.

When the timer stops, a buzzer starts making noise and also feeds the other 2 relays that controls the second & third motor.

> Motors can only be stopped manually by either the security button or through the web interface. If Motor 2 or Motor 3 are stopped via the web interface, they will stop any action taken after the timer stops.

### Modes

| Position | Name   | Temperature | Time |
| -------- | ------ | ----------- | ---- |
| 0        | Off    | 0ºC         | 0m   |
| 1        | Peanut | 180ºC       | 20m  |
| 2        | Cocoa  | 140ºC       | 33m  |
| 3        | Coffee | 170ºC       | 12m  |

> The default state of the switch does not set a timer. Timer response can also be turned off by setting the state of the switch to the position 0 (i.e. the default state.)

## Project structure

The project structure is as follows:

| Resource                                    | Description                                                                                                                                      |
| ------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------ |
| [src/](src)                                 | The [main.cpp](/src/main.cpp) file with the code to be uploaded to esp32                                                                         |
| [data/](data)                               | Static files written directly to the SPI flash file storage (SPIFFS)                                                                             |
| [lib/](lib)                                 | All additional libraries. Core libraries are installed via PlatformIO or written in **lib_deps** using the [platformio.ini](platformio.ini) file |
| [server/](server)                           | [Express](https://expressjs.com/) server for debugging                                                                                           |
| [env&#x2011;template.h](src/env-template.h) | Environment variables template file used to get the credentials for WiFi & VPN                                                                   |
| [platformio.ini](platformio.ini)            | PlatformIO project configuration file                                                                                                            |

### Envionment Variables

Use env vars to store your credentials and build the firmware locally.

1 - Run the command: `mv src/env-template.h src/env.h`

2 - Edit the values

```cpp
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"
```

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

The web interface can _read_ all values, and can only _write_ to the motor states and timer values.

OTA updates are available thanks to [ElegantOTA](https://github.com/ayushsharma82/ElegantOTA).

| Resource | Description                                                                                                                  |
| -------- | ---------------------------------------------------------------------------------------------------------------------------- |
| /events  | Event Source with `readings`, `timer` & `states` events                                                                      |
| /data    | **GET** - Request to update the temperature & humidity readings, timer remaining time and motors states on the web interface |
| /motors  | **POST** - Request to control the state of the motors throught the web interface                                             |
| /time    | **POST** - Request to increase or reduce the timer by 60 seconds                                                             |
| /reset   | **POST** - Request to perform a remote software reset of the ESP32                                                           |
| /update  | Firmware & Filesystem OTA updates                                                                                            |

## Wiring

> **TODO**: Upload circuit, PCB design, 3D design, Gerber files & ESP32 pinout
