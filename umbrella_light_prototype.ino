/*
    HTTP over TLS (HTTPS) example sketch

    This example demonstrates how to use
    WiFiClientSecure class to access HTTPS API.
    We fetch and display the status of
    esp8266/Arduino project continuous integration
    build.

    Limitations:
      only RSA certificates
      no support of Perfect Forward Secrecy (PFS)
      TLSv1.2 is supported since version 2.4.0-rc1

    Created by Ivan Grokhotkov, 2015.
    This example is in public domain.
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char* ssid = "jillian";
const char* password = "2155277800";

const char* host = "api.weatherbit.io";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char fingerprint[] PROGMEM = "B0 31 A2 D0 0E 24 9C 5C DD 46 5B EF D6 BC 30 89 AD 36 81 75";

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);

}

void loop() {

    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/v2.0/forecast/daily?&lat=40.7128&lon=-74.0060&days=3&key=853f8c0513614f118ae53582265be70b";
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
 

    // Check HTTP status
      char status[32] = {0};
      client.readBytesUntil('\r', status, sizeof(status));
      if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
        return;
      }
    
      // Skip HTTP headers
      char endOfHeaders[] = "\r\n\r\n";
      if (!client.find(endOfHeaders)) {
        Serial.println(F("Invalid response"));
        return;
      }
      
 
      // Allocate JsonBuffer
      // Used arduinojson.org/assistant to compute the capacity.
      const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(35) + 1410;
      DynamicJsonBuffer jsonBuffer(bufferSize);
    
      // Parse JSON object
      Serial.println("Parsing JSON");
    
      JsonObject& root = jsonBuffer.parseObject(client);
      if (!root.success()) {
        Serial.println(F("Parsing failed!"));
        return;
      }

      JsonArray& data = root["data"];

      int weather_code = data[0]["weather"]["code"];
      Serial.println(weather_code);

      pinMode(12, OUTPUT);
      
      if(weather_code < 700 || weather_code == 900){
        digitalWrite(12, HIGH);
      }
      else{
        digitalWrite(12, HIGH);
      }
      
  Serial.println("closing connection");

  //go to sleep for 12 hours
  //ESP.deepSleep(4.32e7);

  WiFi.disconnect();
  WiFi.forceSleepBegin();
  delay(43200000);
  
  WiFi.forceSleepWake();
  Serial.println("wakeup");
}
