#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>

const char* ssid = "Nope";
const char* password = "pi314420";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String dataToSend = "gae"; // Global variable to store data to send
String receivedData; // Global variable to store received data
bool newDataReceived = false; // Flag to indicate new data received

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
    Serial.println("Data sent to client");
    
    // Clear the dataToSend variable
    dataToSend = "";
  }

  // Check if new data has been received
  if (newDataReceived) {
    // Process received data
    Serial.println("Received data:");
    Serial.println(receivedData);
    // Reset flag and clear received data
    newDataReceived = false;
    receivedData = "";
  }

  // Your other loop code goes here
}

// WebSocket event handler for incoming messages
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    Serial.println("WebSocket client connected");
    connectedClient = client;
  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("WebSocket client disconnected");
    connectedClient = NULL;
  } else if(type == WS_EVT_DATA){
    // Handle incoming data
    Serial.println("Incoming data:");
    receivedData = "";
    for(size_t i = 0; i < len; i++) {
      receivedData += (char)data[i];
    }
    newDataReceived = true; // Set flag to indicate new data received
  }
}
