#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include "DHT.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_BMP085 bmp;

#define DHTPIN 4   
#define DHTTYPE DHT11
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
int count = 25;

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// defining struct
typedef struct TelemetryData
{
    float temperature;
    float altitude;
    float pressure;
    uint8_t humidity;
} TelemetryData;

// declaring struct queue
QueueHandle_t telemetry_queue;

void setup() {
    Serial.begin(600);

    telemetry_queue = xQueueCreate(5, sizeof(TelemetryData));

    if (telemetry_queue != NULL) {
        xTaskCreate(Task_hum_temp_read,
                    "collection_of_humidity_and_temperature_data",
                    2048,
                    NULL,
                    3,
                    NULL);
        xTaskCreate(Task_value_display,
                    "displaying_of_the_collected_values",
                    2048,
                    NULL,
                    2,
                    NULL);
        xTaskCreate(Task_data_transmit,
                    "Sending the value to the ML model",
                    2048,
                    NULL,
                    1,
                    NULL);
    }
}

void loop() {
  Serial.println("hello im void loop");
}

void Task_hum_temp_read(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        if (count == 30){
          count = 28;
        }
        TelemetryData data_acquired;
        // data_acquired.humidity = dht.readHumidity();
        // data_acquired.temperature = bmp.readTemperature();
        // data_acquired.altitude = bmp.readAltitude();
        data_acquired.humidity = (count + 1) * 10;
        data_acquired.temperature = count + 1;
        data_acquired.altitude = 11;
        count++;
        Serial.println("pre send");
        Serial.println("+++++++++++++++++++++++++");
        Serial.println("Humidity: " + String(data_acquired.humidity) + " %");
        Serial.println("Temperature: " + String(data_acquired.temperature) + " C");
        Serial.println("Pressure: " + String(data_acquired.pressure) + " hPa");
        Serial.println("Altitude: " + String(data_acquired.altitude) + " m");
        Serial.println("+++++++++++++++++++++++++");
        // Send data to the queue
        xQueueSend(telemetry_queue, &data_acquired, portMAX_DELAY);

        vTaskDelay(100 / portTICK_PERIOD_MS); 
    }
}

void Task_value_display(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        TelemetryData data_acquired;

        // Receive data from the queue
        if (xQueueReceive(telemetry_queue, &data_acquired, portMAX_DELAY)) {
            Serial.println("Post recieve");
            Serial.println("------------------------");
            Serial.println("Humidity: " + String(data_acquired.humidity) + " %");
            Serial.println("Temperature: " + String(data_acquired.temperature) + " C");
            Serial.println("Pressure: " + String(data_acquired.pressure) + " hPa");
            Serial.println("Altitude: " + String(data_acquired.altitude) + " m");
            Serial.println("------------------------");
        }
        else
          Serial.println("did not recive skill issue");

        vTaskDelay(500 / portTICK_PERIOD_MS); 
    }
}

void Task_data_transmit(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        TelemetryData data_acquired;

        // Receive data from the queue
        if (xQueueReceive(telemetry_queue, &data_acquired, portMAX_DELAY)) {
            // Do something with the data (transmit to ML model)//data may be empty
            Serial.println("Task_data_tranmsmit check");
        }

        vTaskDelay(100 / portTICK_PERIOD_MS); 
    }
}
