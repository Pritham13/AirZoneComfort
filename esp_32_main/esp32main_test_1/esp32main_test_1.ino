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

// QueueHandle_t telemetry_send_queue, fanSpeed_queue;

void fanSpeed_set(int speed){
    analogWrite(en, speed*0.1*255);
    digitalWrite(rmf, LOW);
    digitalWrite(rmb, HIGH);
}
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

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket client connected");
    connectedClient = client;
    int coreId = xPortGetCoreID();
    Serial.printf("line on core %d\n", coreId);

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
    fanSpeed=data[0]-'0';
    fanSpeed_set(fanSpeed);
    Serial.println(fanSpeed);
  }
}

void setup() {
  if (!bmp.begin()) {
    Serial.println("BMP180 Not Found. CHECK CIRCUIT!");
    while (1) {}}
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

}

void loop() {TelemetryData data_acquired;
        // xQueueReceive(fanSpeed_queue, &fanSpeed, portMAX_DELAY);
        // Serial.println("LOOOP check 1 ");
        // Receive data from the display queue
            // Display data on the OLED display
        data_acquired.humidity = dht.readHumidity();
        data_acquired.temperature = bmp.readTemperature();
        // Serial.println("LOOOP check 2 ");
        data_acquired.pressure = bmp.readPressure()/100;

        // Receive data from the queue
        display.setTextSize(1);
        display.clearDisplay();
        display.setCursor(0,0);
        display.print("Temperature:");
        display.setTextSize(1);
        display.setCursor(0,10);
        display.print(data_acquired.temperature);
        display.setTextSize(1);
        display.setCursor(45,10);
        display.print("C");
        // Serial.println("LOOOP check 3 ");

        display.setTextSize(1);
        display.setCursor(0, 20);
        display.print("Pressure:");
        display.setTextSize(1);
        display.setCursor(0, 30);
        display.print(data_acquired.pressure); // Convert Pa to hPa
        display.setTextSize(1);
        display.setCursor(45, 30);
        display.print("hPa");
        // Serial.println("LOOOP check 4 ");

        // Display humidity
        display.setTextSize(1);
        display.setCursor(0, 40);
        display.print("Humidity:");
        display.setTextSize(1);
        display.setCursor(0, 50);
        display.print(data_acquired.humidity);
        display.setTextSize(1);
        display.setCursor(45, 50);
        display.print("%");
        // Serial.println("LOOOP check 5 ");

        // Display fan speed
        display.setTextSize(1);
        display.setCursor(68, 27); 
        display.print("Fan Speed:");
        display.setTextSize(1);
        display.setCursor(90, 50);
        display.print(fanSpeed);
        display.setTextSize(1);
        display.display();
          if (flag) {
            // if (count == 30.0){
            //       count = 28.5;
            //     }
            //     TelemetryData data_acquired;
            // data_acquired.humidity = (count + 1) * 10;
            // data_acquired.temperature = count + 1;
            // Serial.print("temp int ");
            // Serial.println(data_acquired.humidity);
            char temperatureStr[10];
            char humidityStr[4];
            char dataToSend[10];
            dtostrf(data_acquired.temperature, 3, 1, temperatureStr);
            sprintf(humidityStr, "%02d", data_acquired.humidity);
            // Serial.print("temp ");
            // Serial.println(humidityStr);
            strcpy(dataToSend, temperatureStr); // Copy temperatureStr to dataToSend
            strcat(dataToSend, humidityStr); 
            // Serial.println(dataToSend);
            connectedClient->text(dataToSend);
          }
        }
    


