// #include <Arduino_FreeRTOS.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>

Adafruit_BMP085 bmp;

#define DHTPIN 4   
#define DHTTYPE DHT11
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define rmf 12
#define rmb 13
#define en 5
#define seaLevelPressure_hPa 1013.25
const char* ssid = "Nope";
const char* password = "pi314420";
int fanSpeed = 0;
int flag = 0;
DHT dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient *connectedClient = NULL;

typedef struct TelemetryData
{
    float temperature;
    float altitude;
    float pressure;
    uint8_t humidity;
} TelemetryData;

QueueHandle_t telemetryQueue;

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
void fanSpeed_set(int speed){
    analogWrite(en, speed*0.1*255);
    digitalWrite(rmf, LOW);
    digitalWrite(rmb, HIGH);
}

void displayTask(void *pvParameters) {
    TelemetryData data_acquired;
    int coreId = xPortGetCoreID();
        Serial.printf("data displaying on oled on core %d\n", coreId);
    for (;;) {
        // Read sensor data
        data_acquired.humidity = dht.readHumidity();
        data_acquired.temperature = bmp.readTemperature();
        data_acquired.pressure = bmp.readPressure() / 100;
        displayData(data_acquired);
        // Send data to the queue
        xQueueSend(telemetryQueue, &data_acquired, portMAX_DELAY);

        // Wait for 1000ms
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void websocketTask(void *pvParameters) {
    TelemetryData receivedData;
    int coreId = xPortGetCoreID();
        Serial.printf("data sending on core %d\n", coreId);
    // char dataToSend[15];
    // char temperatureStr[4];
    // char humidityStr[3];
    char dataToSend[15];
    for (;;) {
        // Receive data from the queue
        if (xQueueReceive(telemetryQueue, &receivedData, portMAX_DELAY) == pdTRUE) {
            // Format data
            snprintf(dataToSend, sizeof(dataToSend), "%.1f%02d%", receivedData.temperature, receivedData.humidity);
            // Serial.println(dataToSend);
            if (flag) {
                connectedClient->text(dataToSend);
            }
        }

        // Wait for 1000ms
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.println("WebSocket client connected");
        connectedClient = client;
        if (connectedClient != NULL) {
            flag = 1;
        }
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.println("WebSocket client disconnected");
        flag = 0;
        connectedClient = NULL;
    } else if (type == WS_EVT_DATA) {
        Serial.print("Received data from client:");
        for (size_t i = 0; i < len; i++) {
            Serial.print((char)data[i]);
        }
        Serial.println();
        fanSpeed = data[0] - '0';
        fanSpeed_set(fanSpeed);
        // Serial.println(fanSpeed);
    }
}

void setup() {
    if (!bmp.begin()) {
        Serial.println("BMP180 Not Found. CHECK CIRCUIT!");
        while (1) {}
    }
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    server.begin();
    Serial.println("check");

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("SSD1306 allocation failed"));
    }
    display.clearDisplay();
    display.setTextColor(WHITE);

    // Create telemetryQueue
    telemetryQueue = xQueueCreate(1, sizeof(TelemetryData));

    // Create displayTask
    xTaskCreatePinnedToCore(displayTask, "DisplayTask", 4096, NULL, 1, NULL,0);

    // Create websocketTask
    xTaskCreatePinnedToCore(websocketTask, "WebsocketTask", 4096, NULL, 1, NULL,1);
}

void loop() {
    // This loop should be left empty as tasks handle the functionality
}
