// Import required libraries
#include <ESP8266WiFi.h>
#include <aREST.h>
#include <Wire.h>
#include <Servo.h>

// Create the servo objects
Servo L_Motor; 
Servo R_Motor;
 
// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char* ssid = "NetworkNameHere";
const char* password = "PasswordHere";

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
