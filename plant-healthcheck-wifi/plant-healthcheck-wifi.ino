#include "ESP8266WiFi.h"
#include <ESP8266WebServer.h>
#include "pages.h"

ESP8266WebServer server(80);

void handleRoot();              // function prototypes for HTTP handlers
void handleLogin();
void handleNotFound();

String readFromSerialIfAvailable();
void processSerialCommand(String cmd);
void configState();
void operationState();
bool sendDataToTCPServer(String message);

String ssid;
String password;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Module started");

  server.on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/login", HTTP_POST, handleLogin); // Call the 'handleLogin' function when a POST request is made to URI "/login"
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  WiFi.begin();
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}

void loop() {
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
  server.handleClient();                     // Listen for HTTP requests from clients

  String serialCmd = readFromSerialIfAvailable();

  if (serialCmd.length() > 0) {
    processSerialCommand(serialCmd);  
  }
}

void processSerialCommand(String cmd) {
  char cmdChar = cmd[0];
  String message = "";

  if (cmd.length() >2) {
    message = cmd.substring(2, cmd.length());
  }
  
  if (cmdChar == '0') {
    configState();
  }

  if (cmdChar == '1') {
    operationState();
  }

  if (cmdChar == '2') {
    sendDataToTCPServer(message);
  }
}

void configState() {
  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP("ESPsoftAP_01", "123456789");
  if(!result)
  {
    Serial.println("Failed!");

    return;
  }

  Serial.println("Ready");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();                            // Actually start the server
  Serial.println("HTTP server started");
}

void operationState() {
  server.stop();
  WiFi.softAPdisconnect (true);
  
  WiFi.setAutoReconnect(false);
  WiFi.disconnect();
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
}

bool sendDataToTCPServer(String message) {
  WiFiClient client;
  const char * host = "192.168.88.207";
  const int httpPort = 3333;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return false;
  }

  client.print("aa-1;0;1.1;2.2;3.11");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }

  delay(500);

  client.stop();
  
  return true;
}

void handleRoot() {                          // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<form action=\"/login\" method=\"POST\"><input type=\"text\" name=\"username\" placeholder=\"Username\"></br><input type=\"password\" name=\"password\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form><p>Try 'John Doe' and 'password123' ...</p>");
}

void handleLogin() {                         // If a POST request is made to URI /login
  if( ! server.hasArg("username") || ! server.hasArg("password") 
      || server.arg("username") == NULL || server.arg("password") == NULL) { // If the POST request doesn't have username and password data
    server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }
//  if(server.arg("username") == "John Doe" && server.arg("password") == "password123") { // If both the username and the password are correct
//    server.send(200, "text/html", "<h1>Welcome, " + server.arg("username") + "!</h1><p>Login successful</p>");
//  } else {                                                                              // Username and password don't match
//    server.send(401, "text/plain", "401: Unauthorized");
//  }

  ssid = server.arg("username");
  password = server.arg("password");
  server.send(200, "text/html", "<h1>connecting to: " + server.arg("username") + " " + server.arg("password"));
}

void handleNotFound(){
  String s = MAIN_page;
  server.send(200, "text/html", s); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

String readFromSerialIfAvailable() {
  String responseBuffer = "";
  
  if (Serial.available()) {
    while (Serial.available()) {
      char charIn = Serial.read();
      responseBuffer += charIn;
    }  
  }

  responseBuffer.trim();

  return responseBuffer;
}
