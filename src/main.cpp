#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

#include "SPIFFS.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "DHT.h"
#include "max6675.h"

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
JSONVar readings;                   // JSON Variable to Hold Sensor Readings
JSONVar timer;                      // JSON Variable to Hold Time Values
JSONVar states;                     // JSON Variable to Hold Motor States Values

LiquidCrystal_I2C lcd(0x27, 16, 2); // addr, width (16), height(2) -> 16x2 LCD

DHT dht(DHT_PIN, DHT22);
MAX6675 thermocouple(5, 23, 19); // SCK, CS, SO

int temperature, humidity;

const char *mainTitle = "Tostador                ";

int timerCount;         // Used to count the number of timers that have run
int counter;            // Used to count (decrease) from totalTimeInSeconds to 0
int totalTimeInSeconds; // Used to hold the total number of seconds to run
bool timerIsOn = false;
bool timerResponseIsActive = false;
int isTimeA, isTimeB, isTimeC;

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
  String json = JSON.stringify(readings);
  return json;
}

// Get Time Values and return JSON object
String getTimeValues()
{
  // JSON
  timer["total"] = String(totalTimeInSeconds);
  timer["time"] = String(counter);
  String json = JSON.stringify(timer);
  return json;
}

// Get Motor States and return JSON object
String getMotorStates()
{
  states["motor1"] = !!digitalRead(MOTOR1_PIN);
  states["motor2"] = !!digitalRead(MOTOR2_PIN);
  states["motor3"] = !!digitalRead(MOTOR3_PIN);

  String json = JSON.stringify(states);
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
  Wire.begin(21, 22);
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
    JSONVar data;
    data["timer"] = getTimeValues();
    data["readings"] = getSensorReadings();
    String json = JSON.stringify(data);
    request->send(200, "application/json", json);
    json = String(); });

  events.onConnect([](AsyncEventSourceClient *client)
                   {
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000); });
  server.addHandler(&events);

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
    time = time + String(" Maní"); // TODO: handle acentos
  }
  else if (isTimeB)
  {
    time = time + String(" Café");
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
void handleTimer()
{
  if (timerCount > 0 && timerIsOn)
  {
    if (counter >= 0)
    {
      int minutes = floor(counter / 60);
      int remainingSeconds = counter - minutes * 60;

      lcd.setCursor(0, 0);
      lcd.print(formatTime(minutes, remainingSeconds));

      counter--;
    }

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
    noTone(BUZZER_PIN);
  }

  // If there is a switch after the counter finished, handle the response
  if (timerResponseIsActive)
  {
    handleBuzzer();
    digitalWrite(MOTOR2_PIN, HIGH);
    digitalWrite(MOTOR3_PIN, HIGH);
  }
}

// Handle actions depending on the temperature
void handleTemperature()
{
  // TODO: maybe refactor to avoid infinity
  // 100-150-190 but 30-40-50 for debugging
  int tempLimit = isTimeA ? 30 : isTimeB ? 40
                             : isTimeC   ? 50
                                         : INFINITY;

  // TODO: Refactor to detect change from <tempLimit to >=tempLimit
  if (temperature >= tempLimit)
  {
    digitalWrite(MOTOR1_PIN, HIGH);
  }

  // -- No need to detect change since a minute switch can be set after the motor started
  // -- If a timer is already running, don't set it again
  if (temperature >= tempLimit && !timerIsOn)
  {
    // Only set a timer if there is a switch selected
    bool thereIsAMinuteSwitch = isTimeA || isTimeB || isTimeC;
    if (thereIsAMinuteSwitch)
    {
      // 12-15-18 but 0.3 for debugging
      totalTimeInSeconds = isTimeA ? 0.3 * 60 : isTimeB ? 0.3 * 60
                                            : isTimeC   ? 0.3 * 60
                                                        : 0;
      counter = totalTimeInSeconds;
      timerCount++;
      timerIsOn = true;
    }

    // TODO: add +1 minute or -1 minute with 2 extra inputs
  }
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
  events.send(getMotorStates().c_str(), "states", millis());

  // DEBUG TIME
  Serial.println("Timer data:");
  Serial.println(counter);
  Serial.println(timerIsOn);
  Serial.println(timerCount);

  handleTemperature();
  handleTimer();

  // DEBUG SWITCH
  Serial.println("Switch 1 2 3:");
  Serial.println(isTimeA);
  Serial.println(isTimeB);
  Serial.println(isTimeC);
  Serial.println();

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