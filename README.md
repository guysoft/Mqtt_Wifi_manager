Mqtt_Wifi_manager
=================

This is a hackathon result.
One this is flashed, its all self-contained. You can set up an MQTT client that will push updates via a wifi of your choice.
No hardcoded strings. You can also reset the settings by pressing "reset" sevral quick times.


Hardware Requirements
======================
1. ESP8266 (I used Node MCU 1.0). One that has an analog pin and an SPIFFS.
2. A moisture sensor with an analog pin [one that looks like this](https://www.amazon.com/XCSOURCE-Moisture-Automatic-Watering-TE215/dp/B00ZR3B60I).

Hardware Setup
==============

1. Connect pin VCC of the moisture sensor to VCC on your ESP8266
2. Connect pinn GND of the moisture sensor to the GND on your ESP8266
3. Connect A0 pin on the moisture sensor to the A0 pin on your ESP8266

Software
========

Arduino Libs used:
* DoubleResetDetect
* ArduinoJson (version 5.x, does not work with version 6.x+)
* WiFiManager

Flash mqtt_wifi_manager_moisture_sensor.ino to the ESP.

You are done, thats is it!

Usage
=====

1. Press reset sevral times, a wifi access point should appear with the name ESPxxxxxxxx (where x is the id number of the ESP).
2. Connect to that access point and go to [http://192.168.4.1](http://192.168.4.1). You should see a webpage to set up.
3. Pick your access point you wish to connect to and set up the password.
4. Set a mqtt server and mqtt topic to send to
5. Save, and your ESP should connect and start sending the moisture data as a percentage
6. If you want to enter the setup mode again, press the reset button quickly for sevral times, and you should get the access point page again.

Thats it, enjoy. This is a crude hackaton code, so if you like it and want to PR and make it stable you are more than welcome!

