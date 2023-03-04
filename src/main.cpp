#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "WiFiClient.h"
#include "ESP8266WebServer.h"
#include <Arduino_JSON.h>


/*
/// HTTP client errors
#define HTTPC_ERROR_CONNECTION_FAILED   (-1)
#define HTTPC_ERROR_SEND_HEADER_FAILED  (-2)
#define HTTPC_ERROR_SEND_PAYLOAD_FAILED (-3)
#define HTTPC_ERROR_NOT_CONNECTED       (-4)
#define HTTPC_ERROR_CONNECTION_LOST     (-5)
#define HTTPC_ERROR_NO_STREAM           (-6)
#define HTTPC_ERROR_NO_HTTP_SERVER      (-7)
#define HTTPC_ERROR_TOO_LESS_RAM        (-8)
#define HTTPC_ERROR_ENCODING            (-9)
#define HTTPC_ERROR_STREAM_WRITE        (-10)
#define HTTPC_ERROR_READ_TIMEOUT        (-11)
*/

/* global variables for server connection */
const char *ssid = "Verano_Gast";     // Enter SSID
const char *password = "Verano2010"; // Enter Password
const char* serverName = "http://172.26.203.220:9999/";


/* global variables */
volatile bool data_sent = false;
long unsigned int current_time = millis();
long unsigned int delay_ms = 10000;
const byte pin_D7 = 13;
const int str_len = 30;
char str[str_len];



void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Wait for ESP junk to stop before using the Serial 
  Serial.setTimeout(2000);
  while(!Serial) { }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting...");
  }

  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP()); // Print the IP address

  int i;
  for(i=0; i<str_len; i++)
  {
    str[i] = NULL;
  }
}

void loop()
{

  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);
  int httpResponseCode = http.GET();
  String payload = "{}";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
    Serial.println(payload);

    JSONVar muOb = JSON.parse(payload);
    JSONVar keys = muOb.keys();
    for (int i = 0; i < keys.length(); i++)
    {
      JSONVar value = muOb[keys[i]];
      Serial.print(keys[i]);
      Serial.print(" = ");
      Serial.println(value);
    }

    delay(1000);
    
    char data_name[] = "Weight";
    int data_reading = digitalRead(pin_D7);
    sprintf(str, "{\"%s\":\"%d\"}", data_name, data_reading);

    http.addHeader("Content-Type", "application/json");
    http.POST(str);

    data_sent = true;
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
  delay(2000);

  if(data_sent) 
  {
    Serial.println("I'm awake, but I'm going into deep sleep mode until RESET pin is connected to a LOW signal");
    ESP.deepSleep(0); 
  }
}
