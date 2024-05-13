/*
This code is to send AirQuality to connect platform
----------
Board: ESP32 Node MCU
=========================
Refer to BasicHTTPSClient.ino in examples
==========================
Created on 13 May 2024
by Nicolas Traut, Roberto Toro & Rajeev Mylapalli
*/ 

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "credentials.h"


String HOST = "https://connect-project.io";
String URLget =  "/oauth/user";
String URLsend = "/parse/classes/GameScore";

unsigned long previousMillis = 0;
const long interval = 600000; // Interval to sample sensor data (60 seconds)
int sensorValue;

WiFiClient client;
HTTPClient https;

void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  delay(100);
  analogReadResolution(12);

  Serial.print("Connecting to ");
  Serial.println(SSID_NAME);
  WiFi.begin(SSID_NAME, SSID_PW); //Connecting to WiFi Network
  Serial.println(xPortGetCoreID());

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  getID();
}

void getID() {
  /*
  To get UserID
  curl --request GET \
    --url $CONNECT_URL/oauth/user \
    --header 'Authorization: Bearer '$access_token

  Response:
  { "id": "xxxx" }
  */
  https.begin(HOST + URLget);
  https.setAuthorizationType("");
  https.addHeader("Authorization", " Bearer " + TOKEN);

  int httpCode = https.GET();
  Serial.print(httpCode);

  if (httpCode > 0) {
    String payload = https.getString(); // Get the request response payload
    Serial.println(payload);
  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
    delay(30000);
    getID();
  }
  https.end();
}

void sendData() {
  sensorValue = analogRead(34); //sensor connected to D34
  
  /*
  curl --request POST \
  --url $CONNECT_URL/parse/classes/GameScore \
  --header 'content-type: application/json' \
  --header 'x-parse-application-id: '$PARSE_APPLICATION \
  --header 'Authorization: Bearer '$access_token \
  --data '{
	"score":1337,
	"playerName":"sample",
	"cheatMode":false
  }'
  */
  https.begin(HOST + URLsend);
  https.addHeader("content-type", "application/json");
  https.addHeader("x-parse-application-id", "connect");
  https.addHeader("Authorization", " Bearer " + TOKEN);

  String payload = "{\n";
  payload += " \"score\": " + String(sensorValue) + ",\n";
  payload += "  \"playerName\": \"NaatAirQuality\",\n";
  payload += "  \"cheatMode\": false\n";
  payload += "}\n";

  int httpCode = https.POST(payload);
  if (httpCode>0){
    String response = https.getString();   //Get the request response payload
    Serial.println(response); 
    https.end();      
  }
  else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }
  https.end();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sendData();
  }
}

