#include <WiFi.h>
char *ssid = "moto_g54";
char *password = "prit4102";
void setup(){
    Serial.begin(115200);
    Wifi.begin(ssid,password);
    while (Wifi.status() != WL_CONNECTED){
        delay(500);
        Serial.println("...");
    }
    Serial.print("Wifi connected with IP: ");
    Serial.println(Wifi.localIP());
    
    
}
void loop(){
    // to read the data from stm32 
    
}