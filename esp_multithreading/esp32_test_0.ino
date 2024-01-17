//implementation of queue left
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

//defining struct
typedef struct TelemteryData
{
    float temperature;
    float altitude;
    float pressure;
    uint8_t humidity;
} TelemteryData;

TelemteryData data_acquired; //declating struct

QueueHandle_t struct_queue;

void setup(){
    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        }
    aquired_data_queue=xQueueCreate(5, //Queue length
                            sizeof(int));
    if (aquired_data_queue != NULL){
        xTaskCreate(Task_hum_temp_read,
                    "collection_of_humidity_and_temperature_data",
                    129,//stack size//needs to be changed 
                    NULL,
                    1,//priority //needs to be decided
                    NULL);
        xTaskCreate(Task_value_display,
                    "displaying_of_the_collected_values",
                    129,//stack size//needs to be changed 
                    NULL,
                    2,//priority //needs to be decided
                    NULL);
        }
}

void loop{};


void Task_hum_temp_read(void *pvParameters){
    (void) pvParameters;
    for(;;){
        data_arquired.humidity = dht.readHumidity();
        data_arquired.temperature = bmp.readTemperature();
        data_arquired.altitude = bmp.readAltitude();
        
    }
}
void Task_value_display(void *pvParameters){
    (void)pvParameters;
    for(;;){
        oled.clearDisplay();

        char display_buffer[20];

        // Display Humidity
        sprintf(display_buffer, "Humidity: %.2f %%", humidity);
        oled.setCursor(0, 10);
        oled.println(display_buffer);

        // Display Temperature
        sprintf(display_buffer, "Temp: %.2f C", temperature);
        oled.setCursor(0, 24);
        oled.println(display_buffer);

        // Display Pressure
        sprintf(display_buffer, "Pressure: %.2f hPa", pressure);
        oled.setCursor(0, 36);
        oled.println(display_buffer);

        // Display Altitude
        sprintf(display_buffer, "Altitude: %.2f m", altitude);
        oled.setCursor(0, 48);
        oled.println(display_buffer);

        oled.display();

        vTickDelay(2000); // Adjust delay as needed
        
    }
}