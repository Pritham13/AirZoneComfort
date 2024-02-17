

// #include <Arduino_FreeRTOS.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>


// Pin definitions
#define DHTPIN 4   
#define DHTTYPE DHT11
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define rmf 18
#define rmb 19
#define en 15
#define seaLevelPressure_hPa 1013.25
// Sensor objects
Adafruit_BMP085 bmp; // Barometric Pressure and Temperature Sensor
DHT dht(DHTPIN, DHTTYPE); // DHT11 Temperature and Humidity Sensor

// WiFi credentials
// const char* ssid = "moto_g54";
// const char* password = "prit4102";
const char* ssid = "Nope";
const char* password = "pi314420";

// Global variables
int fanSpeed = 0;
int flag = 0;

// Display object
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);

// AsyncWebServer and WebSocket objects
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient *connectedClient = NULL;

// Structure to hold telemetry data
typedef struct TelemetryData
{
    float temperature;
    float altitude;
    float pressure;
    uint8_t humidity;
} TelemetryData;

// Queue to store telemetry data
QueueHandle_t telemetryQueue,DisplayQueue;

// Function to display sensor data on OLED
void displayData(const TelemetryData &data) {
    // Clear the display
    display.clearDisplay();

    // Set text size and color
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // Display temperature
    display.setCursor(0, 0);
    display.print("Temperature:");
    display.setCursor(0, 10);
    display.print(data.temperature);
    display.print("C");

    // Display pressure
    display.setCursor(0, 20);
    display.print("Pressure:");
    display.setCursor(0, 30);
    display.print(data.pressure);
    display.print("hPa");

    // Display humidity
    display.setCursor(0, 40);
    display.print("Humidity:");
    display.setCursor(0, 50);
    display.print(data.humidity);
    display.print("%");

    // Display fan speed
    display.setCursor(68, 27);
    display.print("Fan Speed:");
    display.setCursor(90, 50);
    display.print(fanSpeed);

    // Display the display buffer
    display.display();
}

// Function to set fan speed
void fanSpeed_set(int speed){
    int final;
    switch(speed){
      case  0:final = 0;
            break;
      case  1:final = 155;
            break;
      case  2:final = 186;
            break;
      case  3:final = 217;
            break;
      case  4:final = 248;
            break;
      case  5:final = 255;
            break;

    }
    
    analogWrite(en,final);
    Serial.println(final);
    digitalWrite(rmf, LOW);
    digitalWrite(rmb, HIGH);
}

// Task to display sensor data on OLED
void displayTask(void *pvParameters) {
    int coreId = xPortGetCoreID();
    Serial.printf("data displaying on core %d\n", coreId);
    TelemetryData data_acquired;
    for (;;) {
        // Read sensor data
        // data_acquired.humidity = dht.readHumidity();
        // data_acquired.temperature = bmp.readTemperature();
        // data_acquired.pressure = bmp.readPressure() / 100;
        // Display sensor data on OLED
         if (xQueueReceive(DisplayQueue, &data_acquired, portMAX_DELAY) == pdTRUE){
                displayData(data_acquired);
         }

        
        
        // Wait for 1000ms
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
// Task to handle collection of data
void CollectTask(void *pvParameters) {
    int coreId = xPortGetCoreID();
    Serial.printf("data Collecting on core %d\n", coreId);
    TelemetryData data_acquired;
    for (;;) {
        // Read sensor data
        data_acquired.humidity = dht.readHumidity();
        data_acquired.temperature = bmp.readTemperature();
        data_acquired.pressure = bmp.readPressure() / 100;
        // Send data to the queue
        xQueueSend(telemetryQueue, &data_acquired, portMAX_DELAY);
        xQueueSend(DisplayQueue, &data_acquired, portMAX_DELAY);
        // Wait for 1000ms
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
// Task to handle WebSocket communication
void websocketTask(void *pvParameters) {
    int coreId = xPortGetCoreID();
    Serial.printf("data sending on core %d\n", coreId);
    TelemetryData receivedData;
    char dataToSend[15];
    for (;;) {
        // Receive data from the queue
        if (xQueueReceive(telemetryQueue, &receivedData, portMAX_DELAY) == pdTRUE) {
            // Format data
            snprintf(dataToSend, sizeof(dataToSend), "%.1f%02d%", receivedData.temperature, receivedData.humidity);
            // Send data to WebSocket client
            if (flag) {
                connectedClient->text(dataToSend);
            }
        }
        // Wait for 1000ms
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Function to handle WebSocket events
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        // WebSocket client connected
        connectedClient = client;
        if (connectedClient != NULL) {
            flag = 1;
        }
    } else if (type == WS_EVT_DISCONNECT) {
        // WebSocket client disconnected
        flag = 0;
        connectedClient = NULL;
    } else if (type == WS_EVT_DATA) {
        // Data received from WebSocket client
        fanSpeed = data[0] - '0';
        Serial.print("Fanspeed recieved:");
        Serial.println(fanSpeed);
        fanSpeed_set(fanSpeed);
    }
}

// Setup function
void setup() {
    pinMode(en,OUTPUT);
    pinMode(rmb,OUTPUT);
    pinMode(rmf,OUTPUT);
    // Initialize sensors
    if (!bmp.begin()) {
        Serial.println("BMP180 Not Found. CHECK CIRCUIT!");
        while (1) {}
    }

    // Initialize serial communication
    Serial.begin(115200);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());

    // Register WebSocket event handler
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    server.begin();

    // Initialize OLED display
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("SSD1306 allocation failed"));
    }
    display.clearDisplay();
    display.setTextColor(WHITE);

    // Create telemetryQueue
    telemetryQueue = xQueueCreate(1, sizeof(TelemetryData));
    DisplayQueue = xQueueCreate(1, sizeof(TelemetryData));

    // Create displayTask
    xTaskCreatePinnedToCore(displayTask, "DisplayTask", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(collectTask, "CollectTask", 4096, NULL, 1, NULL, 0);

    // Create websocketTask
    xTaskCreatePinnedToCore(websocketTask, "WebsocketTask", 4096, NULL, 1, NULL, 1);
}

// Loop function (should be empty as tasks handle functionality)
void loop() {
    // Empty loop
}
