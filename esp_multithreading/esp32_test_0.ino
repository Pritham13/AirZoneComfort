#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

#define DHTPIN 4   
#define DHTTYPE DHT11

//defining struct
typedef struct TelemteryData
{
    float temperature;
    float altitude;
    float pressure;
    uint8_t humidity;
} TelemteryData;

TelemteryData data_acquired; //declating struct

QueueHandle_t arrayQueue;

void setup(){
    aquired_data_queue=xQueueCreate(5, //Queue length
                            sizeof(int));
    if (aquired_data_queue != NULL){
        xTaskCreate(Task_hum_temp_read,
                    "collection_of_humidity_and_temperature_data",
                    129,//needs to be changed 
                    NULL,
                    2,//priority //needs to be decided
                    NULL);
        xTaskCreate(Task_value_display,
                    "displaying_of_the_collected_values",
                    129,//needs to be changed 
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
        
    }
}