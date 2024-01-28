#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>

const char* ssid = "Nope";
const char* password = "pi314420";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String receivedData; // Global variable to store received data
AsyncWebSocketClient *connectedClient = NULL; // Global variable to store the connected client

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
  // No need for loop content in this case as we're handling WebSocket events asynchronously
}

// WebSocket event handler for incoming messages
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket client connected");
    connectedClient = client; // Update connectedClient with the new client
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("WebSocket client disconnected");
    connectedClient = NULL; // Reset connectedClient when client disconnects
  } else if (type == WS_EVT_DATA) {
    // Handle incoming data
    Serial.println("Incoming data:");
    receivedData = "";
    for (size_t i = 0; i < len; i++) {
      receivedData += (char)data[i];

    }
    Serial.println(receivedData);
    
    // Respond to the received data
    if (receivedData == "data1") {
      connectedClient->text("Response to data1");
    } else if (receivedData == "data2") {
      connectedClient->text("Response to data2");
    } else if (receivedData == "data3") {
      connectedClient->text("Response to data3");
    } else {
      connectedClient->text("Unknown data");
    }
  }
}
