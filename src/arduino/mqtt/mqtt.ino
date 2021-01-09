#include <WiFiNINA.h>
#include <PubSubClient.h>

char ssid[] = "SuperSmartSonMesh_2G";
char password[] = "22872287228722872";
const char *mqtt_server = "broker.emqx.io";
int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

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

void Reconnect()
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

void Publish_Message(String topic, String msg)
{
    if(client.connected())
    {
        client.publish(topic.c_str(), msg.c_str());
        Serial.println("publish -> " + topic + " : " + msg);
    }
}

void Subscribe_Topic(String topic)
{
    if(client.connected())
    {
        client.subscribe(topic.c_str());
    }
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

void setup()
{
    Serial.begin(9600);      
    setup_wifi();          
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void loop()
{
    String topic("thsvkd/hello");
    String msg("hello?");

    Subscribe_Topic((topic + "/answer").c_str());
    if(!client.connected())
        Reconnect();

    Publish_Message(topic, msg);

    client.loop();
    delay(1000);
}
