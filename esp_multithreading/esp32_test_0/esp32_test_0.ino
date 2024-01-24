#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_BMP085 bmp;

#define DHTPIN 4   
#define DHTTYPE DHT11
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

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
    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
    }

    telemetry_queue = xQueueCreate(5, sizeof(TelemetryData));

    if (telemetry_queue != NULL) {
        xTaskCreate(Task_hum_temp_read,
                    "collection_of_humidity_and_temperature_data",
                    129,
                    NULL,
                    1,
                    NULL);
        xTaskCreate(Task_value_display,
                    "displaying_of_the_collected_values",
                    129,
                    NULL,
                    2,
                    NULL);
        xTaskCreate(Task_data_transmit,
                    "Sending_the_value_to_the_ML_model",
                    129,
                    NULL,
                    3,
                    NULL);
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
        xQueueSend(telemetry_queue, &data_acquired, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}

void Task_value_display(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        TelemetryData data_acquired;

        // Receive data from the queue
        if (xQueueReceive(telemetry_queue, &data_acquired, portMAX_DELAY)) {
            oled.clearDisplay();

            char display_buffer[20];

            // Display Humidity
            sprintf(display_buffer, "Humidity: %.2f %%", data_acquired.humidity);
            oled.setCursor(0, 10);
            oled.println(display_buffer);

            // Display Temperature
            sprintf(display_buffer, "Temp: %.2f C", data_acquired.temperature);
            oled.setCursor(0, 24);
            oled.println(display_buffer);

            // Display Pressure (assuming it's in TelemetryData struct)
            sprintf(display_buffer, "Pressure: %.2f hPa", data_acquired.pressure);
            oled.setCursor(0, 36);
            oled.println(display_buffer);

            // Display Altitude
            sprintf(display_buffer, "Altitude: %.2f m", data_acquired.altitude);
            oled.setCursor(0, 48);
            oled.println(display_buffer);

            oled.display();

            vTaskDelay(pdMS_TO_TICKS(2000)); 
        }
    }
}

void Task_data_transmit(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        TelemetryData data_acquired;

        // Receive data from the queue
        if (xQueueReceive(telemetry_queue, &data_acquired, portMAX_DELAY)) {
        }

        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}
