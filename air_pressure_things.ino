#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <Wire.h>
#include <jsonlib.h>
#include <TimerEvent.h>
#include "DHT.h"
#define DHTPIN 2

#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE, 15);

String apiKey = "Enter your Write API key from ThingSpeak"; // Enter your Write API key from ThingSpeak
const char *ssid = "simon"; // replace with your wifi ssid and wpa2 key
const char *pass = "password";
const char* server = "api.thingspeak.com";
float number_wind ;
float humidity ;
float pressure;
float temph;
float humidity_local;
ESP8266WiFiMulti WiFiMulti;
WiFiClient client;
// Create  TimerEvent instances
TimerEvent timerOne;
void setup()
{
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


  delay(3000);
  dht.begin();
  Serial.println("REBOOT");
}

void loop()
{

  if ((WiFiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    Serial.print("[HTTP] begin...");
    // configure traged server and url
    // get the entry_id from AirVisual
http://api.airvisual.com/v2/nearest_city?key=yourKey//
    http.begin("http://api.openweathermap.org/data/2.5/weather?q=Turan&appid=yourKey"); //HTTP
    Serial.print("[HTTP] begin...");
    Serial.println("[HTTP] GET...");
    // start connection and send HTTP header
    Serial.print("http://api.openweathermap.org/data/2.5/weather?q=yourKey");
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been sent and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
        String weather_list = jsonExtract(payload, "wind");
        float number_wind_ms = jsonExtract(weather_list, "speed").toFloat();
        
        Serial.print("Wind speed..");
        number_wind = (number_wind_ms * 18) / 5;
        Serial.println(number_wind); //Wind speed in KM/H
        pressure = jsonExtract(payload, "pressure").toFloat();
        humidity_local= jsonExtract(payload, "humidity").toFloat();
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
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s", http.errorToString(httpCode).c_str());

      }
      http.end();
    }
    if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
    {
      //Serial.println(number_wind); //Wind speed
      String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(temph, 2);
      postStr += "&field2=";
      postStr += String(pressure, 2);
      postStr += "&field3=";
      postStr += String(humidity, 2);
      postStr += "&field4=";
      postStr += String(number_wind, 2);
      postStr += "&field5=";
      postStr += String(humidity_local, 2);
      postStr += "\r\n\r\n\r\n\r\n";

      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postStr.length());
      client.print("\n\n");
      client.print(postStr);

      Serial.println("");
      Serial.println("Data Sent to Thingspeak");
    }
    client.stop();
    Serial.println("Waiting...");
    Serial.println("");
    Serial.println("***************************************************");
  }
  Serial.println("I'm awake, but I'm going to delay for some time");
  delay(1000ul * 60 * 5); // Pause for 5 mins.

}
