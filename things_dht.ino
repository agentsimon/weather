
#include "DHTesp.h"

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

DHTesp dht;
#include <jsonlib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h> 
#include <ESP8266HTTPClient.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
float number_wind ;
float humidity ;
float pressure;
float temph;
float local_humidity;
float local_temp;
float visibility;
float local_pressure;
ESP8266WiFiMulti WiFiMulti;
WiFiClient wiFiClient;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
int number1 = 0;
int number2 = random(0,100);
int number3 = random(0,100);
int number4 = random(0,100);
String myStatus = "";


void setup() {
  Serial.begin(115200);  // Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(wiFiClient);  // Initialize ThingSpeak
  String thisBoard = ARDUINO_BOARD;
  Serial.println(thisBoard);

  // Autodetect is not working reliable, don't use the following line
  // dht.setup(17);
  // use this instead:
  dht.setup(2, DHTesp::DHT22); // Connect DHT sensor to GPIO 17
}

void loop() {

  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  delay(dht.getMinimumSamplingPeriod());

  float house_humidity = dht.getHumidity();
  float house_temperature = dht.getTemperature();

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(house_humidity, 1);
  Serial.print("\t\t");
  Serial.print(house_temperature, 1);
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;

    Serial.print("[HTTP] begin...");
    // configure traged server and url
    // get the entry_id from Open weather
http://api.openweathermap.org/data/2.5/weather?q=Turan&appidkey=yourKey//
    http.begin(wiFiClient, "http://api.openweathermap.org/data/2.5/weather?q=Turan&appid=yourKey"); //HTTP
    Serial.print("[HTTP] begin...");
    Serial.println("[HTTP] GET...");
    // start connection and send HTTP header
    Serial.print("http://api.openweathermap.org/data/2.5/weather?q=Turan&appid=yourKey");
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
        local_humidity = jsonExtract(payload, "humidity").toFloat();
        visibility = jsonExtract(payload, "visibility").toFloat();
        Serial.println("....");
        Serial.print("Local pressure is..");
        Serial.println(local_pressure);
        Serial.print("House humidity is..");
        Serial.println(house_humidity);
        Serial.print("House temperature is..");
        Serial.println(house_temperature);
        Serial.print("Local humidity is..");
        Serial.println(local_humidity);
        Serial.print("Local temperature is..");
        Serial.println(local_temp - 273);
        Serial.print("Local wind speed is..");
        Serial.println(number_wind);
        Serial.print("Local visibility is..");
        Serial.println(visibility);
        // set the fields with the values
        ThingSpeak.setField(1, house_humidity);
        ThingSpeak.setField(2, house_temperature);
        ThingSpeak.setField(3, local_temp);
        ThingSpeak.setField(4, number_wind);
        ThingSpeak.setField(5, local_pressure);
        ThingSpeak.setField(6, local_humidity);
        ThingSpeak.setField(7, visibility);

        // figure out the status message
        if (number1 > number2) {
          myStatus = String("field1 is greater than field2");
        }
        else if (number1 < number2) {
          myStatus = String("field1 is less than field2");
        }
        else {
          myStatus = String("field1 equals field2");
        }

        // set the status
        ThingSpeak.setStatus(myStatus);

        // write to the ThingSpeak channel
        int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        if (x == 200) {
          Serial.println("Channel update successful.");
        }
        else {
          Serial.println("Problem updating channel. HTTP error code " + String(x));
        }


        Serial.println("....sleep for 10 minutes");
        delay(1000ul*60*10); // Wait 10 minutes seconds to update the channel again
      }
    }
  }
}
