// Import required libraries
#include <ESP8266WiFi.h>
#include <aREST.h>
#include <Wire.h>
#include <Servo.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Create the servo objects
Servo L_Motor; 
Servo R_Motor;
 
// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char* ssid = "ColdSpringsLabs";
const char* password = "FAF7E0864B";

// The port to listen for incoming TCP connections 
#define LISTEN_PORT           80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Function
int stop(String message);
int forward(String message);
int right(String message);
int left(String message);
int backward(String message);
bool currentLineIsBlank;

void setup(void)
{  
  // Start Serial
  Serial.begin(115200);

  pinMode(14, OUTPUT);
  pinMode(12, OUTPUT);
  
  // And connect 2 servo motors to pins 2 & 4 !
  //L_Motor.attach(14);
  //R_Motor.attach(12);

  L_Motor.write(90);
  R_Motor.write(90);

  // Functions          
  rest.function("stop", stop);
  rest.function("forward", forward);
  rest.function("left", left);
  rest.function("right", right);
  rest.function("backward", backward);
      
  // Give name and ID to device
  rest.set_id("1");
  rest.set_name("robot");
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  
  // Handle REST calls
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }
  rest.handle(client);
  ArduinoOTA.handle();

  while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");       
          }
          client.println("</html>");
           break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
}

int stop(String command) {
  // Stop
  Serial.println("Stopping");
  L_Motor.write(90);
  R_Motor.write(90);
  L_Motor.detach();
  R_Motor.detach();
}

int forward(String command) {
  // GO!
  Serial.println("Going!");
  L_Motor.attach(14);
  R_Motor.attach(12);
  L_Motor.write(180);
  R_Motor.write(0);
}

int left(String command) {
  // Left
  Serial.println("Lefting");
  L_Motor.attach(14);
  R_Motor.attach(12);
  //L_Motor.write(90);
  R_Motor.write(0);
}

int right(String command) {
  // Right
  Serial.println("Righting");
  L_Motor.attach(14);
  R_Motor.attach(12);
  L_Motor.write(180);
  //R_Motor.write(90);
}

int backward(String command) {
  // Back the truck up!
  Serial.println("Backing");
  L_Motor.attach(14);
  R_Motor.attach(12);
  L_Motor.write(0);;
  R_Motor.write(180);
}
