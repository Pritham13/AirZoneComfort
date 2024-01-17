#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>

const char* ssid = "TP-Link_BBF8";
const char* password = "51121921";

AsyncWebServer server(80);

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
      message = request->getParam("message", true)->value();//stores the recived data 
      Serial.println("Received message: " + message);
    }
    request->send(200, "text/plain", "Message received successfully");//sends data to the client 
  });

  // Start the server
  server.begin();
}

void loop() {
  // Nothing to do here
}
