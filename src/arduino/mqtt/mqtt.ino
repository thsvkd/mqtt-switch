#include <WiFiNINA.h>
//#include <PubSubClient.h>
#include <ArduinoMqttClient.h>

char ssid[] = "SuperSmartSonMesh_2G";
char password[] = "22872287228722872";
const char *mqtt_server = "broker.emqx.io";
int mqtt_port = 1883;

WiFiClient wifiClient;
MqttClient client(wifiClient);

void setup_wifi()
{
    delay(100);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        WiFi.begin(ssid, password);
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void connect()
{
    Serial.print("connecting...");

    while (!client.connect("arduino", "try", "try"))
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println("\nconnected!");

    //client.subscribe("thsvkd/hello/answer");
    //client.subscribe("/juhaki.park__Relay_set");
}

void reconnect()
{
    Serial.print("Attempting MQTT connection...");
    while (!client.connected())
    {
        Serial.print(".");
        client.connect("thsvkd");
        delay(500);
    }
    Serial.println("Connect success!");
    Serial.println(client.state());
}

void publish(String topic, String msg)
{
    client.publish(topic.c_str(), msg.c_str());
    Serial.println("publish -> " + topic + " : " + msg);
}

void subscribe(String topic)
{
    client.subscribe(topic.c_str());
}

void messageReceived(String &topic, String &payload)
{
    Serial.println("Message arrived -> topic : " + topic + ", msg : " + payload);
}

void callback(char *topic, byte *payload, unsigned int length)
{
    String msg = "";
    for (int i = 0; i < length; i++)
    {
        msg += (char)payload[i];
    }
    Serial.println("Message arrived -> topic : " + String(topic) + ", msg : " + String(msg));
}

String topic("thsvkd/hello");
String msg("hello?");

void setup()
{
    Serial.begin(115200);
    setup_wifi();
    client.begin(mqtt_server, mqtt_port, wifiClient);
    client.onMessage(messageReceived);
    //client.setServer(mqtt_server, mqtt_port);
    //client.setCallback(callback);

    //delay(1000);
    connect();
    subscribe((topic + "/answer").c_str());
}

void loop()
{
    if (!client.connected())
        reconnect();

    //publish(topic, msg);

    client.loop();
    //Serial.println(client.state());
    //delay(500);
}