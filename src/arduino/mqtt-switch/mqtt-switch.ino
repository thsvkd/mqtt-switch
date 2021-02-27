#include <Servo.h>
#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ESP8266_ESP12)
#include <ESP8266WiFi.h>
#endif
#include <ArduinoOTA.h>
#include "arduino_secrets.h" 

#define SERVO_PIN_1 2
#define SERVO_PIN_2 3
#define LED_PIN 4

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

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
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    //Initialize serial and wait for port to open:
    Serial.begin(9600);

    // attempt to connect to Wifi network:
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    while (WiFi.begin(ssid, pass) != WL_CONNECTED)
    {
        // failed, retry
        Serial.print(".");
        delay(250);
    }

    Serial.println("You're connected to the network");
    Serial.println();

    // You can provide a unique client ID, if not set the library uses Arduin-millis()
    // Each client must have a unique client ID
    // mqttClient.setId("clientId");

    // You can provide a username and password for authentication
    // mqttClient.setUsernamePassword("username", "password");

    // By default the library connects with the "clean session" flag set,
    // you can disable this behaviour by using
    // mqttClient.setCleanSession(false);

    // set a will message, used by the broker when the connection dies unexpectantly
    // you must know the size of the message before hand, and it must be set before connecting
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

    // set the message receive callback
    mqttClient.onMessage(onMqttMessage);

    Serial.print("Subscribing to topic: ");
    Serial.println(inTopic);
    Serial.println();

    // subscribe to a topic
    // the second parameter set's the QoS of the subscription,
    // the the library supports subscribing at QoS 0, 1, or 2
    int subscribeQos = 1;

    mqttClient.subscribe(inTopic, subscribeQos);

    String hello_msg = "thsvkd/hello";

    mqttClient.beginMessage(hello_msg, hello_msg.length(), false, 1, false);
    mqttClient.print(hello_msg);
    mqttClient.endMessage();

    // topics can be unsubscribed using:
    // mqttClient.unsubscribe(inTopic);
    setup_servo();

    ArduinoOTA.begin(WiFi.localIP(), "Arduino_Nano_33_IoT", "thsxogud1", InternalStorage);

    digitalWrite(LED_BUILTIN, HIGH);

    servo.detach();
    servo2.detach();
}

void loop()
{
    check_connect()

    mqttClient.poll();
    ArduinoOTA.poll();
}

// TODO: complete function...
void check_connect()
{
    if (!mqttClient.connect(broker, port))
    {
        if(WiFi.begin(ssid, pass) != WL_CONNECTED)
        {
            while (WiFi.begin(ssid, pass) != WL_CONNECTED)
            {
                Serial.print(".");
                delay(250);
            }
        }
    }
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

        servo2.attach(SERVO_PIN_2);
        servo2.write(90 - 35);
        delay(250);
        servo2.write(90);
        delay(250);

        mqttClient.beginMessage(outTopic, payload.length(), retained, qos, dup);
        mqttClient.print(payload);
        mqttClient.endMessage();

        servo2.detach();
    }
    else if(!recv_msg.compareTo("1"))
    {
        Serial.println("switch on!");
        switch_on = 1;
        
        servo.attach(SERVO_PIN_1);
        servo.write(90 + 35);
        delay(250);
        servo.write(90);
        delay(250);

        mqttClient.beginMessage(outTopic, payload.length(), retained, qos, dup);
        mqttClient.print(payload);
        mqttClient.endMessage();

        servo.detach();
    }
}
  
