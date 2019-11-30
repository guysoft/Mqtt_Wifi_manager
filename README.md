Mqtt_Wifi_manager
=================

This is a hackathon result. Which has been cleand up a bit since.

One this is flashed, its all self-contained. You can set up an MQTT client that will push updates via a wifi of your choice.
No hardcoded strings. You can also reset the settings by pressing "reset" sevral quick times.

The supported sensors are:

* Moisture sensore
* Tempreture sensor DS18B20
* PH sensor - [like this one](https://www.botshop.co.za/how-to-use-a-ph-probe-and-sensor/)

[You can see the project in action in this video](http://www.youtube.com/watch?v=zjSd5B13HYE)


Hardware Requirements
======================
1. ESP8266 (I used Node MCU 1.0). One that has an analog pin and an SPIFFS.
2. A sensor
  * A moisture sensor with an analog pin [one that looks like this](https://www.amazon.com/XCSOURCE-Moisture-Automatic-Watering-TE215/dp/B00ZR3B60I) (will also [work with this](https://www.amazon.com/XCSOURCE-Moisture-Automatic-Watering-TE215/dp/B00ZR3B60I), but its not recommended because they die after about 24 hours in water, for more info see [this](https://www.youtube.com/watch?v=udmJyncDvw0).
  * [Tempreture sensor using DS18B20](https://www.amazon.com/ELENKER-Waterproof-Temperature-Thermometer-Resistance/dp/B01DQQPR2A/) - they also go really cheap on Aliexpress and ebay.
  

Hardware Setup
==============

Moisture sensor
---------------

1. Connect pin VCC of the moisture sensor to VCC on your ESP8266
2. Connect pin GND of the moisture sensor to the GND on your ESP8266
3. Connect A0 pin on the moisture sensor to the A0 pin on your ESP8266


Tempreture sensor
-----------------
1. Connect pin VCC of the moisture sensor to VCC on your ESP8266
2. Connect pin GND of the moisture sensor to the GND on your ESP8266
3. Connect the data pin to pin D4


PH sensor
---------

1. Connect pin VCC of the moisture sensor to VCC on your ESP8266
2. Connect pin GND of the moisture sensor to the GND on your ESP8266
2. Connect second pin GND of the moisture sensor to the GND on your ESP8266
3. Connect P0 pin on the moisture sensor to the A0 pin on your ESP8266
4. Preform [calibraion of POTs as explained here](https://www.botshop.co.za/how-to-use-a-ph-probe-and-sensor/)



Software
========

Arduino Libs used (all can be downloaded from the Library Manager):
* [DoubleResetDetect](https://github.com/jenscski/DoubleResetDetect)
* [ArduinoJson](https://arduinojson.org/v5/doc/installation/) (version 5.x, does not work with version 6.x+)
* [WiFiManager](https://github.com/tzapu/WiFiManager)
* [PubSub](https://github.com/knolleary/pubsubclient)


Tempreture sensor libraries
---------------------------

* DallasTemperature
* OneWire



Flashing
--------

When flashing, set in your Arduino IDE the flash size to be 4M and above, you want to have flah space to start the saved settings of wifi manager.

For moisture sensor
-------------------


Flash [mqtt_wifi_manager_moisture_sensor.ino](https://github.com/guysoft/Mqtt_Wifi_manager/blob/master/mqtt_wifi_manager_moisture_sensor/mqtt_wifi_manager_moisture_sensor.ino) to the ESP.

For tempreture sensor
---------------------

Flash [mqtt_wifi_manager_temp_sensor.ino](https://github.com/guysoft/Mqtt_Wifi_manager/blob/master/mqtt_wifi_manager_temp_sensor/mqtt_wifi_manager_temp_sensor.ino) to the ESP.

For PH sensor
-------------

Flash [mqtt_wifi_manager_temp_sensor.ino](https://github.com/guysoft/Mqtt_Wifi_manager/blob/master/mqtt_wifi_manager_ph_sensor/mqtt_wifi_manager_ph_sensor.ino) to the ESP.

You are done, thats is it!

Usage
=====

1. Press reset sevral times, a wifi access point should appear with the name ESPxxxxxxxx (where x is the id number of the ESP).
2. Connect to that access point and go to the router IP. For example, if your phone/laptop connected to 192.148.4.xxx then connect to [http://192.168.4.1](http://192.168.4.1). You should see a webpage to set up. Note: the IP might change depending on your library settings. So see what IP you are assigned.
3. Pick your access point you wish to connect to and set up the password.
4. Set a mqtt server and mqtt topic to send to
5. Save, and your ESP should connect and start sending the moisture data as a percentage
6. If you want to enter the setup mode again, press the reset button quickly for sevral times, and you should get the access point page again.

Thats it, enjoy. Some of this is still a crude hackaton code, so if you like it and want to PR and make it stable you are more than welcome!

