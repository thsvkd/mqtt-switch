# mqtt-switch

This project is a project that allows you to remotely adjust the switch using mqtt.

## requirements

### Hardware

- Arduino Nano 33 IoT board
- SG-90 servo motor
- 3M strong double-sided sticker

### library

- ArduinoMqttClient

## How to install

The basic principle is to attach the servomotor to the switch with strong tape and then press the switch with the force the servomotor presses to turn the light on and off. So, you need to secure a space around the switch to stick double-sided stickers, and then attach the servo motor strongly.

1. modify arduino_secrets.h and insert your wifi name and password
2. upload mqtt-switch.ino file to your 33 IoT board
3. Attach servo motor around switch and board, battery
4. download android app and install app
5. execute app and press ON, OFF butten
6. Enjoy!
