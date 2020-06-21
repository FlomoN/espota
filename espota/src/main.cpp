#include <Arduino.h>
#include <mqtthelp.h>

const char *ssid = "UPC6769240";                      // The SSID (name) of the Wi-Fi network you want to connect to
const char *password = "65Xezmxtjwye";                // Password for your WiFi connection
const char *mqtt_server = "192.168.0.5";              // Address of your MQTT Broker
const char *clientName = "testbench";                 // Name of this device for registering to the broker
const char *otaProvider = "http://192.168.0.61:3001"; // Hostname or IP + Port of your OTA Provider

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup()
{
  MQTTHelp::channels.push_back("test");
  MQTTHelp::setup(ssid, password, mqtt_server, clientName, otaProvider, callback);
}

void loop()
{
  MQTTHelp::loop();
}