#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>          // Library for DHT sensors
#include <SimpleTimer.h>
void reconnect();
#define DHT_PIN 5
#define DHTTYPE DHT11

char ssid[] = "hydra";
char wifiPassword[] = "*******";

#define mqtt_server "fa76c8cb489a4a8da8c45925f257b263.s1.eu.hivemq.cloud"  // server name or IP
#define mqtt_user "espriella"      // username
#define mqtt_password "*******";   // password
#define mqtt_server_port 8883  //server port

WiFiClientSecure espClient;
PubSubClient client(espClient);

float sensor_temperature; 
float sensor_humidity; 
int  publish_period = 1000; 

DHT dht(DHT_PIN, DHTTYPE);    
SimpleTimer timer;

void updateReadings(){ 
    if (!client.connected()) {
      reconnect();
    }  
    sensor_temperature = dht.readTemperature();
    sensor_humidity = dht.readHumidity();
    Serial.print("[HUMIDITY] " + String(sensor_humidity) + " ");
    Serial.print("[TEMPERATURE] " + String(sensor_temperature) + " ");
    Serial.println();
    char s[50];
    sprintf(s, "{humedad:%f, temperatura:%f}", sensor_humidity, sensor_temperature);
    client.publish("sensores",s, true);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("Connected!");
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_server_port); 
  client.connect("ESP32Client", mqtt_user, mqtt_password);
  timer.setInterval(publish_period, updateReadings);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.print("[Error] Not connected: ");
      Serial.println(client.state());
      Serial.println("Wait 5 seconds before retry.");
      delay(5000);
    }
  }
}

void loop() {
  timer.run();
}
