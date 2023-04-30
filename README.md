# Cocoa Roaster

A project to control a cocoa roaster with an esp32 (ESP32-DEVKIT-V1).
The project uses PlatformIO to write and load the esp32 code, using the following directory structure:

- The [**src**](/src) folder contains the [main.cpp](/src/main.cpp) file, which is the file with the code to be uploaded to esp32.

- The folder [**data**](/data) contains all static files (html, css, js and png/ico)
which are written directly to SPI flash file storage (SPIFFS).

- The folder [**lib**](/lib) has all the additional libraries. Core libraries are installed via PlatformIO or
written in **lib_deps** inside the [platformio.ini](platformio.ini) file.

## Logic behind

The sensors consist of a thermometer (type K thermocouple) and a humidity sensor (DHT11).

- When the temperature reaches 190ºC it feeds a relay that controls the first motor,
and also starts a timer that can be 12, 15 or 18 minutes depending on the state of a 3 state switch.
    - the first motor can only be stopped manually.
    - the first state of the switch sets a 12m timer.
    - the second state of the switch sets a 15m timer.
    - the third state of the switch sets an 18m timer.
    - the default state of the switch does not set a timer.

- When the timer stops, a buzzer starts making noise and also feeds another relay that controls the second motor.
    - the buzzer can only be stopped by switching to the default state (turning off all states)
    - the second motor can only be stopped manually.
  
## Timer Logic

The switch must be adjusted before the temperature reaches 190ºC. At that point, there are a couple of rules:

1. If a timer is started and then changed to a higher time, the resulting time is: newTime - runningTime
  
      ex. The timer is set to 18 min, has 9:45 running (8:15 left), and is changed to 12 min.
      the result will be 12 - 9:45 = 2:15 remaining.
    
      - If the result is < 0, then the timer should be stopped.
    
        ex. The timer is set to 18 min and has 16:45 running (1:15 left) and is changed to 15 min.
        the result will be 15 - 16:45 = -1:15 remaining.
    
2. If a timer is started and then changed to a lower time, the resulting time is: newTime - runningTime
 
      ex. The timer is set to 12 min and has 9:45 running (2:15 left) and is changed to 18 min.
      the result will be 18 - 9:45 = 8:15 remaining.
  
## Display

All information from the sensors is passed to both an I²C LCD display and a web server.
