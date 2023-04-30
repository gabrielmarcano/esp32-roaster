# Tostador de Cacao

A project to control a cocoa toaster with an esp32 (ESP32-DEVKIT-V1). 
The project uses PlatformIO to write and upload the code to the esp32, using the following directory structure:

- The [**src**](/src) folder contains the [main.cpp](/src/main.cpp) file which is the file with the code that is going to be uploaded to the esp32.

- The [**data**](/data) folder holds all static files (html, css, js & png/ico) 
which are directly written into the SPI flash file storage (SPIFFS).

- The [**lib**](/lib) folder have all additional libraries. Main libraries are installed through PlatformIO or 
written into **lib_deps** inside the [platformio.ini](platformio.ini) file. 

## Logic Behind

The sensors consist in a thermometer (Type K thermocouple) and a humidity sensor (DHT11). 

- When the temperature reaches 190ºC it powers a relay controlling the first motor, 
and also start a timer which can be 12, 15 or 18 minutes depending of the state of a 3-state switch.
  - the first motor can only be stopped manually.
  - the first state of the switch sets a 12m timer.
  - the second state of the switch sets a 15m timer.
  - the third state of the switch sets a 18m timer.
  - the default state of the switch doesn't set timer.

- When the timer stops, a buzzer starts to make noise and it also powers another relay controlling the second motor.
  - the buzzer can only be stopped by turning off the switch.
  - the second motor can only be stopped manually
  
### Timer
---

The switch should be set before the temperature reaches 190ºC. At that time, there are a couple of rules:

1. If a timer starts and then it's changed to a higher time, the resulting time is: newTime - runningTime
  
    ex. The timer is set to 18m, and it has 9:45 running (8:15 remaining), and is changed to be 12min, 
    the result will be 12 - 9:45 = 2:15 remaining.
    
    - If the result is < 0, then the timer should stop.
    
      ex. The timer is set to 18m, and it has 16:45 running (1:15 remaining), and is changed to be 15min, 
      the result will be 15 - 16:45 = -1:15 remaining.
    
2. If a timer starts and then it's changed to a lower time, the resulting time is: newTime - runningTime
 
    ex. The timer is set to 12m, and it has 9:45 running (2:15 remaining), and is changed to be 18min, 
    the result will be 18 - 9:45 = 8:15 remaining.
  
## Visualization

All the sensors information is passed to both an I²C LCD and a Web Server.
