#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

#include "SPIFFS.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <dht11.h>
#include "max6675.h"

#define DHT_PIN 18
#define MOTOR_PIN 2 // por prueba colocamos el led interno

AsyncWebServer server(80);          // Create AsyncWebServer object on port 80
AsyncEventSource events("/events"); // Create an Event Source on /events
JSONVar readings;                   // Json Variable to Hold Sensor Readings

LiquidCrystal_I2C lcd(0x27, 16, 2); // addr, width (16), height(2) -> 16x2 LCD

dht11 DHT11;
MAX6675 thermocouple(5, 23, 19); // SCK, CS, SO

bool motorStatus = LOW;

// Get Sensor Readings and return JSON object
String getSensorReadings()
{
  // Update readings
  DHT11.read(DHT_PIN);
  int temperature = (int)thermocouple.readCelsius();

  // LCD
  lcd.setCursor(0, 1); // Set the cursor to column 0, line 1
  lcd.printf("T: %dC H: %d%%   ", temperature, DHT11.humidity);

  // JSON
  readings["temperature"] = String(temperature);
  readings["humidity"] = String(DHT11.humidity);
  String jsonString = JSON.stringify(readings);
  return jsonString;
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
}

// Initialize LCD
void initLCD(const char *title)
{
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.print(title);
}

void setup()
{
  Serial.begin(115200);
  pinMode(MOTOR_PIN, OUTPUT);
  initLCD("Tostador d cacao");
  initWifi("GUEST", "88426259");
  initSPIFFS();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  server.serveStatic("/", SPIFFS, "/");

  // Request for the latest sensor readings
  server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String json = getSensorReadings();
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

void loop()
{
  events.send("ping", NULL, millis());
  events.send(getSensorReadings().c_str(), "new_readings", millis());

  delay(4000);
}

/*
TIMER

T1 = 12m
T2 = 15m
T3 = 18m

else = APAGAR EL BUZZER

HACER LOGICA DE DECONTEO

0 O MENOR SE APAGA - ENCIENDE MOTOR 2
*/

/*

LIMITE TEMPERATURA 240

motor enciente en 190

MOTOR NO APAGA

*/