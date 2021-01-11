#include <Servo.h>
#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ESP8266_ESP12)
#include <ESP8266WiFi.h>
#endif

#define SERVO_PIN_1 2
#define SERVO_PIN_2 3

char ssid[] = "SuperSmartSonMesh_2G";
char pass[] = "22872287228722872";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

Servo servo;
Servo servo2;

const char broker[] = "broker.emqx.io";
int port = 1883;
const char willTopic[] = "arduino/will";
const char inTopic[] = "thsvkd/switch";
const char outTopic[] = "thsvkd/switch/result";

unsigned long previousMillis = 0;
String recv_msg = String("");
int switch_on = 0;

int count = 0;

void setup_servo()
{
    Serial.println("servo setup...");
    servo.attach(SERVO_PIN_1);
    servo2.attach(SERVO_PIN_2);

    servo.write(90);
    servo2.write(90);
    Serial.println("servo setup finish!!");
}

void setup()
{
    Serial.begin(9600);
    while (!Serial)
    {
        ;
    }
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    while (WiFi.begin(ssid, pass) != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("You're connected to the network");
    Serial.println();

    String willPayload = "oh no!";
    bool willRetain = true;
    int willQos = 1;

    mqttClient.beginWill(willTopic, willPayload.length(), willRetain, willQos);
    mqttClient.print(willPayload);
    mqttClient.endWill();

    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(broker);

    if (!mqttClient.connect(broker, port))
    {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());

        while (1)
            ;
    }

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();

    mqttClient.onMessage(onMqttMessage);

    Serial.print("Subscribing to topic: ");
    Serial.println(inTopic);
    Serial.println();

    int subscribeQos = 1;

    mqttClient.subscribe(inTopic, subscribeQos);

    setup_servo();
}

void loop()
{
    mqttClient.poll();
}


void onMqttMessage(int messageSize)
{
    recv_msg = String("");
    bool retained = false;
    int qos = 1;
    bool dup = false;
    String payload = "success!!";

    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', duplicate = ");
    Serial.print(mqttClient.messageDup() ? "true" : "false");
    Serial.print(", QoS = ");
    Serial.print(mqttClient.messageQoS());
    Serial.print(", retained = ");
    Serial.print(mqttClient.messageRetain() ? "true" : "false");
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

    while (mqttClient.available())
    {
        recv_msg += (char)mqttClient.read();
    }

    Serial.println(recv_msg);
    Serial.println();

    if(!recv_msg.compareTo("0"))
    {
        Serial.println("switch off!");
        switch_on = 0;

        servo.write(135);
        delay(500);
        servo.write(90);
        delay(500);

        mqttClient.beginMessage(outTopic, payload.length(), retained, qos, dup);
        mqttClient.print(payload);
        mqttClient.endMessage();
    }
    else if(!recv_msg.compareTo("1"))
    {
        Serial.println("switch on!");
        switch_on = 1;

        servo2.write(135);
        delay(500);
        servo2.write(90);
        delay(500);

        mqttClient.beginMessage(outTopic, payload.length(), retained, qos, dup);
        mqttClient.print(payload);
        mqttClient.endMessage();
    }
}
  
