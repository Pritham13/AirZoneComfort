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
void loop (){
    msg = client.readStringUntil('\n');
    Serial.println(msg);
}