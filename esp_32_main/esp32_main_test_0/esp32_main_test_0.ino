// #include <Arduino_FreeRTOS.h>
// #include <queue.h>
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
const char* ssid = "Nope";
const char* password = "pi314420";
#define seaLevelPressure_hPa 1013.25
int fanSpeed = 0;
int flag=0;
DHT dht(DHTPIN, DHTTYPE);


// declare an SSD1306 display object connected to I2C
// Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient *connectedClient = NULL;
// defining struct
typedef struct TelemetryData
{
    float temperature;
    float altitude;
    float pressure;
    uint8_t humidity;
} TelemetryData;
//fuctions
void fanSpeed_set(int speed){
    analogWrite(en, speed*0.1*255);
    digitalWrite(rmf, LOW);
    digitalWrite(rmb, HIGH);
}
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

// declaring struct queue
QueueHandle_t telemetry_send_queue,telemetry_disp_queue,fanSpeed_queue;

void setup() {
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
    // Start the server
    server.begin();

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
      Serial.println(F("SSD1306 allocation failed"));
    }
    display.clearDisplay();
    display.setTextColor(WHITE);

    telemetry_disp_queue = xQueueCreate(1, sizeof(TelemetryData));
    telemetry_send_queue = xQueueCreate(1, sizeof(TelemetryData));
    fanSpeed_queue = xQueueCreate(1, sizeof(fanSpeed));

    if (telemetry_disp_queue != NULL&& telemetry_send_queue != NULL) {
        xTaskCreatePinnedToCore(Task_hum_temp_read,
                    "to collect the temperature and humidity values",
                    4096,
                    NULL,
                    1,
                    NULL,
                    0);
        xTaskCreatePinnedToCore(Task_value_display,
                    "displaying collected values",
                    4096,
                    NULL,
                    2,
                    NULL,
                    0);
        xTaskCreatePinnedToCore(Task_data_transmit,
                    "Sending_the_value_to_the_ML_model",
                    4096,
                    NULL,
                    1,
                    NULL,
                    1);//core to be run on
    }
}

void loop() {}

void Task_hum_temp_read(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        TelemetryData data_acquired;
        data_acquired.humidity = dht.readHumidity();
        data_acquired.temperature = bmp.readTemperature();
        data_acquired.altitude = bmp.readAltitude();

        // Send data to the queue
        xQueueSend(telemetry_disp_queue, &data_acquired, portMAX_DELAY);
        xQueueSend(telemetry_send_queue, &data_acquired, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}

void Task_value_display(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        TelemetryData data_acquired;
        xQueueReceive(fanSpeed_queue, &fanSpeed, portMAX_DELAY);
        // Receive data from the queue
        if (xQueueReceive(telemetry_disp_queue, &data_acquired, portMAX_DELAY)) {
            display.setTextSize(1);
            display.clearDisplay();
            display.setCursor(0,0);
            display.print("Temperature:");
            display.setTextSize(1);
            display.setCursor(0,10);
            // display.print(bmp.readTemperature());
            display.print(data_acquired.temperature);
            display.setTextSize(1);
            display.setCursor(45,10);
            display.print("C");

            display.setTextSize(1);
            display.setCursor(0, 20);
            display.print("Pressure:");
            display.setTextSize(1);
            display.setCursor(0, 30);
            display.print(data_acquired.pressure); // Convert Pa to hPa
            display.setTextSize(1);
            display.setCursor(45, 30);
            display.print("hPa");

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

            // Display fan speed
            display.setTextSize(1);
            display.setCursor(68, 27); 
            display.print("Fan Speed:");
            display.setTextSize(1);
            display.setCursor(90, 50);
            display.print(fanSpeed);
            display.setTextSize(1);


            display.display();
        // delay(1000);

            vTaskDelay(pdMS_TO_TICKS(2000)); 
        }
    }
}

void Task_data_transmit(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        TelemetryData data_acquired;

        // Receive data from the queue
        if (xQueueReceive(telemetry_send_queue, &data_acquired, portMAX_DELAY)) {
          if (connectedClient != NULL) {
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
            char dataToSend[50];
            dtostrf(data_acquired.temperature, 3, 1, temperatureStr);
            sprintf(humidityStr, "%02d", data_acquired.humidity);
            // Serial.print("temp ");
            // Serial.println(humidityStr);
            strcpy(dataToSend, temperatureStr); // Copy temperatureStr to dataToSend
            strcat(dataToSend, humidityStr); 
            Serial.println(dataToSend);
            connectedClient->text(dataToSend);
        }

        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}
