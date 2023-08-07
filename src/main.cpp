#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <AsyncElegantOTA.h>

#include "SPIFFS.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "DHT.h"
#include "max6675.h"

#define LCD_SDA 21
#define LCD_SCL 22
#define MAX_SCK 5
#define MAX_CS 23
#define MAX_SO 19
#define DHT_PIN 18
#define MOTOR1_PIN 2 // 25, but internal LED (2) for debugging
#define MOTOR2_PIN 26
#define MOTOR3_PIN 27
#define BUZZER_PIN 14
#define TIME_A 36
#define TIME_B 34
#define TIME_C 35

AsyncWebServer server(80);          // Create AsyncWebServer object on port 80
AsyncEventSource events("/events"); // Create an Event Source on /events

// As a thumb rule, 32 bytes for every key/value pair inside the json
StaticJsonDocument<64> readings; // JSON Variable to Hold Sensor Readings
StaticJsonDocument<64> timer;    // JSON Variable to Hold Time Values
StaticJsonDocument<128> states;  // JSON Variable to Hold Motor States Values

LiquidCrystal_I2C lcd(0x27, 16, 2); // addr, width (16), height(2) -> 16x2 LCD

DHT dht(DHT_PIN, DHT22);                       // PIN, MODEL
MAX6675 thermocouple(MAX_SCK, MAX_CS, MAX_SO); // SCK, CS, SO

int temperature, humidity; // Hold the current value of the temperature & humidity

const char *mainTitle = "Tostador                ";

int timerCount;                     // Used to count the number of timers that have run
int counter;                        // Used to count (decrease) from totalTimeInSeconds to 0
int totalTimeInSeconds;             // Used to hold the total number of seconds to run
bool timerIsOn = false;             // Represent an active timer
bool timerResponseIsActive = false; // Represent a timer response (after a timer finishes) is active
bool isTimeA, isTimeB, isTimeC;     // Represent the selection of a timer configuration for the 3-state switch. Only one is true

bool motors23Activated = false; // Used to turn on the motors 2 & 3 only once every timer response

const int MAX_TEMP_LIMIT = 1000;        // set a large value for max temperature limit
const float TIMER_DURATION_DEBUG = 0.2; // set a default timer duration for debugging

// Get Sensor Readings and return JSON object
String getSensorReadings()
{
  // Update readings
  humidity = (int)dht.readHumidity();
  temperature = (int)thermocouple.readCelsius();

  // LCD
  lcd.setCursor(0, 1); // Set the cursor to column 0, row 1
  lcd.printf("T: %dC H: %d%%   ", temperature, humidity);

  // JSON
  readings["temperature"] = String(temperature);
  readings["humidity"] = String(humidity);

  String json;
  serializeJson(readings, json);

  return json;
}

// Get Time Values and return JSON object
String getTimeValues()
{
  // JSON
  timer["total"] = String(totalTimeInSeconds);
  timer["time"] = String(counter);

  String json;
  serializeJson(timer, json);

  return json;
}

// Get Motor States and return JSON object
String getMotorStates()
{
  states["motor1"] = !!digitalRead(MOTOR1_PIN);
  states["motor2"] = !!digitalRead(MOTOR2_PIN);
  states["motor3"] = !!digitalRead(MOTOR3_PIN);

  String json;
  serializeJson(states, json);

  return json;
}

// Initialize SPIFFS
void initSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

// Initialize WiFi
void initWifi(const char *ssid, const char *password)
{
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println(".");
  }

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
}

// Initialize LCD
void initLCD(const char *title)
{
  Wire.begin(LCD_SDA, LCD_SCL);
  lcd.init();
  lcd.backlight();
  lcd.print(title);
}

// Initialize Server
void initServer()
{
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  server.serveStatic("/", SPIFFS, "/");

  // Request for the latest sensor readings
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              DynamicJsonDocument data(256);

              // Create the objects
              StaticJsonDocument<64> timerValues;
              StaticJsonDocument<64> readingsValues;
              StaticJsonDocument<128> statesValues;

              // Get values into objects
              deserializeJson(timerValues, getTimeValues());
              deserializeJson(readingsValues, getSensorReadings());
              deserializeJson(statesValues, getMotorStates());
              
              // Add objects to the DynamicJsonDocument
              data["timer"] = timerValues.as<JsonVariant>();
              data["readings"] = readingsValues.as<JsonVariant>();
              data["states"] = statesValues.as<JsonVariant>();
              
              String json;
              serializeJson(data, json);

              request->send(200, "application/json", json);

              json = String(); });

  // Update the latest status of the motors states
  server.on(
      "/motors", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
      {
        StaticJsonDocument<32> response;
        DeserializationError error = deserializeJson(response, (const char *)data, len);

        if (!error)
        {
          if (response.containsKey("motor1"))
          {
            digitalWrite(MOTOR1_PIN, response["motor1"].as<bool>());
          }
          if (response.containsKey("motor2"))
          {
            digitalWrite(MOTOR2_PIN, response["motor2"].as<bool>());
          }
          if (response.containsKey("motor3"))
          {
            digitalWrite(MOTOR3_PIN, response["motor3"].as<bool>());
          }
          events.send(getMotorStates().c_str(), "states", millis());
          request->send(200, "text/plain", "ok");
        }
        else
        {
          request->send(404, "text/plain", error.c_str());
        }
      });

  events.onConnect([](AsyncEventSourceClient *client)
                   {
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000); });
  server.addHandler(&events);

  // Start OTA service
  AsyncElegantOTA.begin(&server);

  // Start server
  server.begin();
}

// Format seconds into mm:ss
String formatTime(int minutes, int seconds)
{
  String time;
  time = (minutes < 10) ? time + String("0") + String(minutes) : time + String(minutes);
  time = time + String(":");
  time = (seconds < 10) ? time + String("0") + String(seconds) : time + String(seconds);
  if (isTimeA)
  {
    time = time + String(" Mani"); // TODO: handle acentos
  }
  else if (isTimeB)
  {
    time = time + String(" Cafe");
  }
  else if (isTimeC)
  {
    time = time + String(" Cacao");
  }
  time = time + String("           ");
  return time;
}

// Make noise with the buzzer
void handleBuzzer()
{
  tone(BUZZER_PIN, 440);
  // tone(BUZZER_PIN, 494, 1000);
  // tone(BUZZER_PIN, 523, 1000);
}

// Handle all timer logic with the 3-state switch
void handleTimerAndResponse()
{
  if (timerCount > 0 && timerIsOn && counter >= 0)
  {

    int minutes = floor(counter / 60);
    int remainingSeconds = counter - minutes * 60;

    lcd.setCursor(0, 0);
    lcd.print(formatTime(minutes, remainingSeconds));

    counter--;

    if (counter < 0)
    {
      timerIsOn = false;
      timerResponseIsActive = true;

      // Print the title again
      lcd.setCursor(0, 0);
      lcd.print(mainTitle);
    }
  }

  // When switch is moved to OFF, then turn off the response
  if (!isTimeA && !isTimeB && !isTimeC)
  {
    timerResponseIsActive = false;
    motors23Activated = false;
    noTone(BUZZER_PIN);
  }

  // If there is a switch after the counter finished, handle the response
  if (timerResponseIsActive)
  {
    handleBuzzer();

    // Only turn the motors once every timer response
    if (!motors23Activated)
    {
      digitalWrite(MOTOR2_PIN, HIGH);
      digitalWrite(MOTOR3_PIN, HIGH);

      motors23Activated = true;

      // Send new motor statuses to server
      events.send(getMotorStates().c_str(), "states", millis());
    }
  }
}

// Handle actions depending on the temperature
void handleTemperature()
{
  int tempLimit = 0;
  float timerDuration = 0;

  if (isTimeA)
  {
    tempLimit = 30;
    timerDuration = TIMER_DURATION_DEBUG ? TIMER_DURATION_DEBUG : 15;
  }
  else if (isTimeB)
  {
    tempLimit = 50;
    timerDuration = TIMER_DURATION_DEBUG ? TIMER_DURATION_DEBUG : 18;
  }
  else if (isTimeC)
  {
    tempLimit = 70;
    timerDuration = TIMER_DURATION_DEBUG ? TIMER_DURATION_DEBUG : 20;
  }
  else
  {
    tempLimit = MAX_TEMP_LIMIT;
  }

  // Check if temperature exceeds the limit and if it is rising & start timer if it's not already running
  static int prevTemp = 0;
  if (temperature >= tempLimit && temperature > prevTemp && prevTemp < tempLimit && !timerIsOn && timerDuration > 0)
  {
    digitalWrite(MOTOR1_PIN, HIGH);

    // Send new motor statuses to server
    events.send(getMotorStates().c_str(), "states", millis());

    // Start the timer
    totalTimeInSeconds = timerDuration * 60;
    counter = totalTimeInSeconds;
    timerCount++;
    timerIsOn = true;
  }
  prevTemp = temperature;
}

// TODO: add +1 minute or -1 minute with 2 extra inputs
// Handle interrupt response
void handleInterrupt()
{
  // if +1 minute button
  totalTimeInSeconds = totalTimeInSeconds + 60;
  counter = counter + 60;
  timerIsOn = true;

  // if -1 minute button
  totalTimeInSeconds = totalTimeInSeconds - 60;
  counter = counter - 60;
}

void setup()
{
  Serial.begin(115200);

  dht.begin();

  pinMode(MOTOR1_PIN, OUTPUT);
  pinMode(MOTOR2_PIN, OUTPUT);
  pinMode(MOTOR3_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TIME_A, INPUT);
  pinMode(TIME_B, INPUT);
  pinMode(TIME_C, INPUT);

  initLCD(mainTitle);
  initWifi("GUEST", "88426259");
  initSPIFFS();
  initServer();

  delay(2000);
}

void loop()
{
  // Get switch readings
  isTimeA = digitalRead(TIME_A);
  isTimeB = digitalRead(TIME_B);
  isTimeC = digitalRead(TIME_C);

  // Send Events to the client with the Sensor Readings
  events.send("ping", NULL, millis());
  events.send(getSensorReadings().c_str(), "readings", millis());
  events.send(getTimeValues().c_str(), "timer", millis());

  // DEBUG TIME
  // Serial.println("Timer data:");
  // Serial.println(counter);
  // Serial.println(timerIsOn);
  // Serial.println(timerCount);

  handleTemperature();
  handleTimerAndResponse();

  // DEBUG SWITCH
  // Serial.println("Switch A B C:");
  // Serial.println(isTimeA);
  // Serial.println(isTimeB);
  // Serial.println(isTimeC);
  // Serial.println();

  delay(1000);
}

/*

MANUAL WAIT

unsigned long readingsLastTime = 0;
unsigned long readingsDelay = 1000;

if ((millis() - readingsLastTime) > readingsDelay)
{
  // Things here will repeat every "readingsDelay" seconds

  readingsLastTime = millis();
}

*/

/**
 * cacao 15 min
 * cafe 15 min
 * mani con concha 20-25 min
 * mani sin concha 15-20 min
 */