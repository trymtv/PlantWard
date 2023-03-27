#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>

#include <DHT.h>
#include <DHT_U.h>

//#include "arduino_secrets.h"

//Moisture sensor consts
int MoistureSensorPin = A1;
int relayPin = 16;
int sensorValue = 0;

int threshold = 300;

//DHT sensor consts
#define dht_pin A0
#define DHTTYPE DHT11
DHT dht(dht_pin, DHTTYPE);

// WiFi consts
char ssid[] = "PlantWard";
char pass[] = "12345678";
int status = WL_IDLE_STATUS;

WiFiClient client;
MqttClient mqttClient(client);
int port = 1883;

char server[] = "64.225.72.132";

unsigned long lastConnectionTime = 0;               // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5L * 1000L;  // delay between updates,

// time values to control pumping time and break time
unsigned long starttime_watering;
unsigned long endtime;

// booleans to control when to water
bool watering = false;
bool one_minute_break = false;



void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();


  //connect to mqtt
  Serial.print("Attempting to connect to the MQTT broker: ");

  Serial.println(server);
  if (!mqttClient.connect(server, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1)
      ;
  }
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  mqttClient.onMessage(onMqttMessage);
  Serial.print("Subscribing to topic: ");
  Serial.println("sensor/threshold");
  Serial.println();
  // subscribe to topic
  mqttClient.subscribe("sensor/threshold");
}



void loop() {
  // poll for new threshold value
  mqttClient.poll();

  // get new moisture value
  sensorValue = analogRead(MoistureSensorPin);


  // wait 30 seconds if it has previously watered
  if (one_minute_break == false) {
    starttime_watering = millis();
    endtime = starttime_watering;
  } else {
    endtime = millis();
    if ((endtime - starttime_watering) >= 35000) {
      one_minute_break = false;
    }
  }

  if (sensorValue < threshold) {
    if (one_minute_break == false) {
      watering = true;
      starttime_watering = millis();
      endtime = starttime_watering;
      while ((endtime - starttime_watering) <= 5000)  // do this loop for up to 5000mS
      {
        // code for pumping
        endtime = millis();
        digitalWrite(relayPin, LOW);
      }
      watering = false;
      one_minute_break = true;
    } else {
      digitalWrite(relayPin, HIGH);
    }
  } else {
    digitalWrite(relayPin, HIGH);
  }
  //DHT sensor
  float t = dht.readTemperature(); /*read temp*/
  float h = dht.readHumidity();    /*read humidity*/

  

  //

  // if ten seconds have passed since last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    Serial.print("Soil humidity = ");
    Serial.print(sensorValue);
    Serial.print(" Air humidity = ");
    Serial.print(h);
    Serial.print("% ");
    Serial.print("Air temperature = ");
    Serial.print(t);
    Serial.println(" Celsius ");
    Serial.println("Current threshold value = " + String(threshold));
    String sensor_json = "{\"air_hum\":" + String(h) + ", \"air_temp\":" + String(t) + ", \"soil_hum\":" + String(sensorValue) + "}";
    mqttClient.beginMessage("sensor/values");
    mqttClient.print(sensor_json);
    mqttClient.endMessage();
    lastConnectionTime = millis();
  }
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    threshold = mqttClient.readString().toInt();
  }
  Serial.println("MqttThreshold =" + String(threshold));
  Serial.println();
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}