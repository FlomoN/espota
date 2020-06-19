#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266httpUpdate.h>

namespace MQTTHelp
{

  const char *ssid;
  const char *password;
  const char *mqtt_server;
  const char *clientName;
  const char *otaProvider;
  std::function<void(char *, uint8_t *, unsigned int)> callback;

  int timer = 0;

  WiFiClient espClient;
  PubSubClient client(espClient);

  /**
   * Sets up WiFi connection
   **/
  void connectWifi()
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password); // Connect to the network
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println(" ...");

    while (WiFi.status() != WL_CONNECTED)
    { // Wait for the Wi-Fi to connect
      delay(1000);
      Serial.print(".");
    }

    Serial.println('\n');
    Serial.println("Connection established!");
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
  }

  /**
   * Handles OTA signals and then calls external callback
   **/
  void otacheck(char *topic, byte *payload, unsigned int length)
  {
    if (strcmp(topic, (std::string("update/") + std::string(clientName)).c_str()) == 0)
    {
      Serial.println("Receiving update...");
      t_httpUpdate_return ret = ESPhttpUpdate.update(otaProvider, 80, (std::string("/bin/") + std::string(clientName) + std::string(".bin")).c_str());

      switch (ret)
      {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        Serial.print(otaProvider);
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
      }
    }
    callback(topic, payload, length);
  }

  /**
   * Handles connection loss to the mqtt Broker and resubscribes to channels
   * TODO: Give in more channels to resubscribe to
   **/
  void reconnect()
  {
    while (!client.connected())
    {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect(clientName))
      {
        Serial.println("connected");
        // Once connected, publish an announcement...
        // client.publish("outTopic", "hello world");
        // ... and resubscribe
        // client.subscribe("inTopic");
        client.subscribe((std::string("update/") + std::string(clientName)).c_str());
      }
      else
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }

  /**
   * send out periodical ping with own client Name every 10 seconds
   **/
  void ping()
  {
    if (millis() - timer > 60000)
    {
      timer = millis();
      client.publish("ping", "teeeestore");
    }
  }

  /**
   * Starting Point 
   **/
  void setup(const char *_ssid, const char *_password, const char *_mqtt_server, const char *_clientName, const char *_otaProvider, std::function<void(char *, uint8_t *, unsigned int)> _callback)
  {
    ssid = _ssid;
    password = _password;
    mqtt_server = _mqtt_server;
    clientName = _clientName;
    callback = _callback;
    otaProvider = _otaProvider;

    Serial.begin(9600); // Start the Serial communication to send messages to the computer
    delay(10);
    Serial.println('\n');
    connectWifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(otacheck);
    reconnect();
  }

  void loop()
  {
    if (!client.connected())
    {
      reconnect();
    }
    client.loop();
    ping();
  }

} // namespace MQTTHelp