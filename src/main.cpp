#include <Arduino.h>
#include <midea_ir.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "BluetoothSerial.h"
#include <ArduinoJson.h>

const uint8_t MIDEA_RMT_CHANNEL = 1;
const uint8_t MIDEA_TX_PIN = 4;

MideaIR ir;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

char incomingChar;
String message = "";

const char* ssid = "Luu Huy";
const char* password = "04052001";
const char *mqtt_server = "192.168.1.10";


WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32_Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("/esp32/control-lilhuy-ac");
      Serial.println("Subscribed to /esp32/control-lilhuy-ac"); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void onMessageReceived(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  // message is a json string {"state": "on", fanLevel: 3, temperature: 23}
  // parse the json string and set the state of the A/C
  StaticJsonDocument<200> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, messageTemp);

  // Test if parsing succeeds
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
  bool state = doc["state"] == "on";
  int fanLevel = doc["fanLevel"];
  int temperature = doc["temperature"];
  
  if(state){
    ir.enabled = true;
    ir.mode = MODE_COOL;
    ir.fan_level = fanLevel;
    ir.temperature = temperature;
    midea_ir_send(&ir);
    Serial.println("Sending IR signal: " + messageTemp);
  }else{
    ir.enabled = false;
    midea_ir_send(&ir);
    Serial.println("Sending IR signal to turn off AC");
  }
}


void setup()
{
  Serial.begin(9600);
  // init library
  midea_ir_init(&ir, MIDEA_RMT_CHANNEL, MIDEA_TX_PIN);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(onMessageReceived);

  SerialBT.begin("May Tho");

  
}

void loop()
{
  //mqtt
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //bluetooth
  if (SerialBT.available())
  {
 
    char incomingChar = SerialBT.read();
    if (incomingChar != '\n')
    {
      message += String(incomingChar);
    }
    else
    {
      message = "";
    }
    Serial.write(incomingChar);
  }
  if (message == "on")
  {
    Serial.println("Sending IR signal to turn on the A/C");
    // set mode, temperature and fan level and internal state to enabled
    ir.enabled = true;
    ir.mode = MODE_COOL;
    ir.fan_level = 3;
    ir.temperature = 23;
    midea_ir_send(&ir);
  }
  else if (message == "off")
  {
    Serial.println("Sending IR signal to turn off the A/C");
    // set state to disabled
    ir.enabled = false;

    // send the IR signal which will turn the A/C off
    midea_ir_send(&ir);
  }
}