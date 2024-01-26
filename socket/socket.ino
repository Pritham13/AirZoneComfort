#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>

const char* ssid = "Nope";
const char* password = "pi314420";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String dataToSend = "gae"; // Global variable to store data to send
AsyncWebSocketClient * connectedClient = NULL; // Global variable to store the connected client

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

  // WebSocket event handlers
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Start the server
  server.begin();
}

void loop() {
  // Check if there is data to send
  if (!dataToSend.isEmpty() && connectedClient != NULL) {
    // Send data to the connected client
    connectedClient->text(dataToSend);
    
    // Clear the dataToSend variable
    dataToSend = "";
  }

  // Your other loop code goes here
}

// WebSocket event handler
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    Serial.println("WebSocket client connected");
    connectedClient = client;
  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("WebSocket client disconnected");
    connectedClient = NULL;
  }
}
