/*
  WriteSingleField

  Description: Writes a value to a channel on ThingSpeak every 20 seconds.

  Hardware: ESP8266 based boards

  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!

  Note:
  - Requires ESP8266WiFi library and ESP8622 board add-on. See https://github.com/esp8266/Arduino for details.
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.

  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.

  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.

  For licensing information, see the accompanying license file.

  Copyright 2018, The MathWorks, Inc.
*/

#include "ThingSpeak.h"
#include "secrets.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266HTTPClient.h>
#include <jsonlib.h>
#include <TimerEvent.h>
#include "DHT.h"
#define DHTPIN 2
char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
const char* server = "api.thingspeak.com";
float number_wind ;
float humidity ;
float pressure;
float temph;
float humidity_local;
float visibility;
ESP8266WiFiMulti WiFiMulti;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE, 15);
int number = 0;


void setup() {

  Serial.begin(115200); // open serial port, set the baud rate to 9600 bps

  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while (!Serial) { }
  delay(10);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  delay(3000);
  dht.begin();
  Serial.println("REBOOT");
}
void loop() {
  HTTPClient http;
  Serial.print("[HTTP] begin...");
  // configure traged server and url
  // get the entry_id from AirVisual
  //http://api.airvisual.com/v2/nearest_city?key=yourKey//
  http.begin("http://api.openweathermap.org/data/2.5/weather?q=Turan&appid=your_key"); //HTTP
  Serial.print("[HTTP] begin...");
  Serial.println("[HTTP] GET...");
  // start connection and send HTTP header
  Serial.print("http://api.openweathermap.org/data/2.5/weather?q=Turan&appid=your_key");
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been sent and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
      String weather = jsonExtract(payload, "main");
      Serial.println(weather);
      String weather_list = jsonExtract(weather, "main");
      Serial.println(weather_list);
      String weather_speed = jsonExtract(payload, "wind");
      Serial.println(weather_speed);
      float number_wind_ms = jsonExtract(weather_speed, "speed").toFloat();

      Serial.print("Wind speed..");
      number_wind = (number_wind_ms * 18) / 5;
      Serial.println(number_wind); //Wind speed in KM/H

      pressure = jsonExtract(payload, "pressure").toFloat();
      humidity_local = jsonExtract(payload, "humidity").toFloat();
      visibility = jsonExtract(payload, "visibility").toFloat();
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      delay(2000);
      humidity = dht.readHumidity();
      // Read temperature as Celsius (the default)
      temph = dht.readTemperature();
      Serial.print("Pressure is..");
      Serial.println(pressure);
      Serial.print("Humidity house is..");
      Serial.println(humidity);
      Serial.print("Humidity local is..");
      Serial.println(humidity_local);
      Serial.print("Temperature is..");
      Serial.println(temph);
      Serial.print("Visibility is..");
      Serial.println(visibility);
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s", http.errorToString(httpCode).c_str());

    }
    http.end();
    ThingSpeak.setField(1, temph);
    ThingSpeak.setField(2, pressure);
    ThingSpeak.setField(3, humidity);
    ThingSpeak.setField(4, number_wind);
    ThingSpeak.setField(5, humidity_local);
    ThingSpeak.setField(6,  visibility);
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    ThingSpeak.setStatus(String("Yes, data is in"));
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    Serial.println("I'm awake, but I'm going to delay for some time");
    delay(1000ul * 60 * 5); // Pause for 5 mins.
  }
}
