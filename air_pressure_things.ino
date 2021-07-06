#include <ESP8266WiFi.h>
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>
#include <ESP8266WiFiMulti.h>//https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <Wire.h>
#include <jsonlib.h>
#include <TimerEvent.h>
#include "DHTesp.h"
#define DHTPIN 2
DHTesp dht;
ESP8266WiFiMulti WiFiMulti;
WiFiClient client;

String apiKey = "93YS4C2UN5QJ0JI3"; // Enter your Write API key from ThingSpeak
const char* server = "api.thingspeak.com";
float number_wind ;
float humidity ;
float pressure;
float temph;
float humidity_local;
float local_temp;
float visibility;
float local_pressure;

// Create  TimerEvent instances
TimerEvent timerOne;
void setup()
{
  Serial.begin(115200); // open serial port, set the baud rate to 9600 bps

  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("connected...yeey :)");
  delay(10);


  delay(3000);
  dht.setup(2, DHTesp::DHT22); // Connect DHT sensor to GPIO 2
  Serial.println("REBOOT");
}

void loop()
{

  if ((WiFiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    Serial.print("[HTTP] begin...");
    // configure traged server and url
    // get the entry_id from Open weather
http://api.openweathermap.org/data/2.5/weather?q=Turan&appidkey=yourKey//
    http.begin("http://api.openweathermap.org/data/2.5/weather?q=Turan&appid=eb1821cfb6c7675f6bbe3ec6f7cb83cf"); //HTTP
    Serial.print("[HTTP] begin...");
    Serial.println("[HTTP] GET...");
    // start connection and send HTTP header
    Serial.print("http://api.openweathermap.org/data/2.5/weather?q=Turan&appid=eb1821cfb6c7675f6bbe3ec6f7cb83cf");
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been sent and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        //Serial.println(payload);
        String weather = jsonExtract(payload, "main");
        //Serial.println(weather);
        String weather_list = jsonExtract(weather, "main");
        // Serial.println(weather_list);
        String weather_speed = jsonExtract(payload, "wind");
        //Serial.println(weather_speed);
        float number_wind_ms = jsonExtract(weather_speed, "speed").toFloat();
        number_wind = (number_wind_ms * 18) / 5;
        local_temp = jsonExtract(payload, "temp").toFloat();
        local_pressure = jsonExtract(payload, "pressure").toFloat();
        humidity_local = jsonExtract(payload, "humidity").toFloat();
        visibility = jsonExtract(payload, "visibility").toFloat();
        // Reading temperature or humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

        delay(dht.getMinimumSamplingPeriod());
        humidity = dht.getHumidity();
        // Read temperature as Celsius (the default)
        temph = dht.getTemperature();
        Serial.println("....");
        Serial.print("Local pressure is..");
        Serial.println(local_pressure);
        Serial.print("House humidity is..");
        Serial.println(humidity);
        Serial.print("House temperature is..");
        Serial.println(temph);
        Serial.print("Local humidity is..");
        Serial.println(humidity_local);
        Serial.print("Local temperature is..");
        Serial.println(local_temp - 273);
        Serial.print("Local wind speed is..");
        Serial.println(number_wind);
        Serial.print("Local visibility is..");
        Serial.println(visibility);
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
      postStr += String(local_pressure, 2);
      postStr += "&field3=";
      postStr += String(humidity, 2);
      postStr += "&field4=";
      postStr += String(local_temp - 273, 2);
      postStr += "&field5=";
      postStr += String(number_wind, 2);
      postStr += "&field6=";
      postStr += String(humidity_local, 2);
      postStr += "&field7=";
      postStr += String(visibility, 0);
      postStr += "\r\n\r\n\r\n\r\n";
      delay(2500);//waitting for reply, important! the time is base on the condition of internet
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

      //client.stop();
      Serial.println("Waiting...");
      Serial.println("");
      Serial.println("***************************************************");

      Serial.println("I'm awake, but I'm going to delay for some time");
      delay(1000ul * 60 * 10); // Pause for 10 mins.
    }
  }
}
