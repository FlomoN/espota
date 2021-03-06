# ESP OTA

This library / template enables you to update your ESP8266 (more maybe coming soon) over-the-air on every new Github release. It's CI/CD for your IOT devices.

Accessible to your ESP you will have to have the [OTA-Provider](http://github.com/flomon/ota-provider) deployed (either publicly or in the same network as your ESP).

Fill out the proper variables on top of the main.cpp file.

```C++
const char *ssid = "your-ssid";             // The SSID (name) of the Wi-Fi network you want to connect to
const char *password = "your-password";         // Password for your WiFi connection
const char *mqtt_server = "192.168.0.5"; // Address of your MQTT Broker
const char *clientName = "testbench";    // Name of this device for registering to the broker
const char *otaProvider = "ota.example.com";    // Hostname or IP + Port of your OTA Provider
```

The `mqtthelp.h` Library will take care of WiFi and MQTT connection. Some default topics are used:

- topic `hello` will send the clientName once the connection to the MQTT broker is established
- topic `ping` will send the clientName every 60 seconds to signal availability
- on topic `update/{clientName}` the [OTA-Provider](http://github.com/flomon/ota-provider) will tell your device to pull the latest binary and update.

To subscribe to more channels add them to the vector `MQTTHelp::channels`. This will ensure resubscription of the channels when the MQTT connection has to be reestablished.

```C++
MQTTHelp::channels.push_back("test");
```

To publish messages use the `MQTTHelp::client` (PubSubClient).

```C++
MQTTHelp::client.publish(topic, message);
```
