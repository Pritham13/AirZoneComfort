/******************************************************SOCKET WORKING*********************************************************************/
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
const char* ssid = "Nope";
const char* password = "pi314420";
typedef struct TelemetryData
{
    float temperature;
    float altitude;
    float pressure;
    uint8_t humidity;
} TelemetryData;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
int flag =0;
float count =28.4;
AsyncWebSocketClient *connectedClient = NULL;

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket client connected");
    connectedClient = client;
    int coreId = xPortGetCoreID();
    Serial.printf("line on core %d\n", coreId);

    // Send initial data to the client when connected
    if (connectedClient != NULL) {
      // connectedClient->text("Initial data from ESP32!");
      flag=1;
    }
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("WebSocket client disconnected");
    flag=0;
    connectedClient = NULL;
  } else if (type == WS_EVT_DATA) {
    Serial.print("Received data from client:");
    for (size_t i = 0; i < len; i++) {
      Serial.print((char)data[i]);
    }
    Serial.println();

    // Process the received data here if needed
  }
}

void sendInitialData() {
  if (connectedClient != NULL) {
    if (count == 30.0){
          count = 28.5;
        }
        TelemetryData data_acquired;
    data_acquired.humidity = (count + 1) * 10;
    data_acquired.temperature = count + 1;
    // Serial.print("temp int ");
    // Serial.println(data_acquired.humidity);
    char temperatureStr[10];
    char humidityStr[4];
    char dataToSend[50];
    // dtostrf(data_acquired.pressure, 2, 0, pressureStr);
    dtostrf(data_acquired.temperature, 3, 1, temperatureStr);
    sprintf(humidityStr, "%02d", data_acquired.humidity);
    // Concatenate data strings
    // Serial.print("temp ");
    // Serial.println(humidityStr);
    // snprintf(dataToSend, sizeof(dataToSend), temperatureStr, humidityStr);
    strcpy(dataToSend, temperatureStr); // Copy temperatureStr to dataToSend
    strcat(dataToSend, humidityStr); 
    Serial.println(dataToSend);
    connectedClient->text(dataToSend);
  }
}

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

  // Wait for a few seconds before sending initial data
  delay(1000);
  
}

void loop() {

    // Send initial data to the client when connected
    if (flag) {
      int coreId = xPortGetCoreID();
      // sendInitialData();
      // Serial.println("data sent");
      delay(1000);
    }
}
