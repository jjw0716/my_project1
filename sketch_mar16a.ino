/*
 *  This sketch sends a message to a TCP server
 *
 */

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS D2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer;


ESP8266WiFiMulti WiFiMulti;

void setup() {
    //Serial.begin(115200);
  Serial.begin(9600);
  
  Serial.println("Dallas Temperature IC Control Library Demo");

  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 

   Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);
 
  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();
    
    delay(10);

    // We start by connecting to a WiFi network
    WiFiMulti.addAP("V35 ThinQ_5984", "253fef0a1a4e");

    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}


void loop() {
    const uint16_t port = 80;
    const uint16_t port2 = 8080;
    const char * host = "api.thingspeak.com"; // ip or dns
    const char * host2 = "ec2-54-180-149-98.ap-northeast-2.compute.amazonaws.com";

  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  
  // It responds almost immediately. Let's print out the data
 float tempC = sensors.getTempC(insideThermometer);
    
    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("connection failed");
        Serial.println("wait 5 sec...");
        delay(5000);
        return;
    }
   String url = "/update?api_key=9OIPCK8DKGPEZVBX&field1=";
  url += tempC;
   client.print(String("GET ") + url + " HTTP/1.1\r\n" + 
                "Host: " + host + "\r\n" +
                "Connection: close\r\n\r\n");
                
    unsigned long timeout = millis();
    while (client.available() == 0){
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client TImeout !");
        client.stop();
        return;
      }
    }

    // This will send the request to the server
   // client.print("Send this data to server");

    //read back one line from server
    //String line = client.readStringUntil('\r');
    //client.println(line);

    Serial.println("closing connection");
    client.stop();

   /// server

   
   Serial.print("connecting to my server");
   Serial.println(host2);

   if (!client.connect(host2, port2)) {
        Serial.println("connection_2 failed");
        Serial.println("wait 5 sec...");
        delay(5000);
        return;
    }

   url = "/temp?tmp=";
   url += tempC;
   client.print(String("GET ") + url + " HTTP/1.1\r\n" + 
                "Host: " + host2 + "\r\n" +
                "Connection: close\r\n\r\n");

   //unsigned long 
   timeout = millis();
    while (client.available() == 0){
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client TImeout !");
        client.stop();
        return;
      }
    }

     Serial.println("closing connection_2");
    client.stop();

    Serial.println("wait 60 sec...");
    delay(60000);
}


void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
