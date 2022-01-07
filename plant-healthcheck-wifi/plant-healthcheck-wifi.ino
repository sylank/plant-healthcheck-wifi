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

  server.on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/config", HTTP_POST, handleDone); // Call the 'handleLogin' function when a POST request is made to URI "/login"
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  connectToNetwork("","");
}

void loop() {
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
  
  connectToNetwork(ssid, password);
  
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
}

void connectToNetwork(String ssid, String password) {
  if (ssid == "" && password == "") {
    WiFi.begin();
  } else {
    WiFi.begin(ssid, password);
  }

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
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

void handleRoot() {
  String s = MAIN_page;
  server.send(200, "text/html", s);
}

void handleDone() {
  if( ! server.hasArg("ssid") || ! server.hasArg("password") 
      || server.arg("ssid") == NULL || server.arg("password") == NULL) {
    String s = ERROR_page;
    server.send(400, "text/html", s);
    return;
  }

  ssid = server.arg("ssid");
  password = server.arg("password");
  Serial.println("Configuration:");
  Serial.println(ssid);
  Serial.println(password);

  String s = DONE_page;
  server.send(200, "text/html", s);

  delay(3000);
  operationState();
}

void handleNotFound(){
  String s = NOT_FOUND_page;
  server.send(200, "text/html", s);
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
