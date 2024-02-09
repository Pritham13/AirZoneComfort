#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>

const char* ssid = "TP-Link_BBF8";
const char* password = "51121921";

AsyncWebServer server(80);

String dataToSend = "gae"; // Global variable to store data to send
char humidityStr[4];
char temperatureStr[10];
void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Define the route to handle the POST request
  server.on("/handle_post", HTTP_POST, [](AsyncWebServerRequest *request){
    String message;
    if (request->hasParam("message", true)) {
      message = request->getParam("message", true)->value();
      Serial.println("Received message: " + message);
      
      // Store the received message in the global variable
      dataToSend = "Received message: " + message;
    }
    request->send(200, "text/plain", "Message received successfully");
  });

  // Start the server
  server.begin();
}

void loop() {
  // Check if there is data to send
  if (!dataToSend.isEmpty()) {
    // Send data to the client
    server.clients().send(dataToSend.c_str());
    
    // Clear the dataToSend variable
    dataToSend = "";
  }

  // Your other loop code goes here
}

