# ESP8266 MQTT JSON SK6812RGBW HomeAssistant

This project shows a super easy way to get started using Digital LED strips with [Home Assistant](https://home-assistant.io/), a sick, open-source Home Automation platform that can do just about anything. 

The code covered in this repository utilizes [Home Assistant's MQTT JSON Light Component](https://home-assistant.io/components/light.mqtt_json/) and an ESP8266 microcontroller. 

This project can run standalone. When turned on the white LEDs will light.  This means you can use the project as a normal light source. If the device can connect to WiFi and MQTT, then you are able to control remotely.


#### Supported Features Include
- RGB Color Selection
- White Selection
- Brightness 
- Effects with Animation Speed
- Over-the-Air (OTA) Upload from the ArduinoIDE!

Most of the effects incorporate the currrently selected color while other effects use pre-defined colors. The input_number and automation in the HA configuration example allow you to easily set a transition speed from HA's user interface without needing to use the Services tool. 

The default speed for the effects is hard coded and is set when the light is first turned on. When changing between effects, the previously used transition speed will take over. If the effects don't look great, play around with the slider to adjust the transition speed (AKA the effect's animation speed). 

#### Optional MOSFET
Addressable LED are different to a normal LED string. Every pixel ALWAYS has power as it is awaiting commands. So even while the string is off. This quiescent current can add up. On a 5-meter strip (60 LEDs per meter) a full watt is used from the strip while the strip is not illuminated. (That’s nearly enough to send a person to the moon)
The sketch and related wiring diagram includes a P-Channel MOSFET which will turn off power to the strip when the strip is off. This means the ESP8266 can always energised and the LED strip will turn on only when it is meant to light up.


#### OTA Uploading
This code also supports remote uploading to the ESP8266 using Arduino's OTA library. To utilize this, you'll need to first upload the sketch using the traditional USB method. However, if you need to update your code after that, your WIFI-connected ESP chip should show up as an option under Tools -> Port -> Porch at your.ip.address.xxx. More information on OTA uploading can be found [here](http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html). Note: You cannot access the serial monitor over WIFI at this point.  


#### Demo Video
[![Demo Video](http://i.imgur.com/cpW2JAX.png)](https://www.youtube.com/watch?v=DQZ4x6Z3678 "Demo - RGB Digital LED Strip controlled using ESP, MQTT, and Home Assistant")
NOTE: This sketch is using a diffrent set of effects.

#### Tutorial Video
[![Tutorial Video](http://i.imgur.com/9UMl8Xo.jpg)](https://www.youtube.com/watch?v=9KI36GTgwuQ "The BEST Digital LED Strip Light Tutorial - DIY, WIFI-Controllable via ESP, MQTT, and Home Assistant")

#### Parts List
- [Digital RGB Leds](https://www.adafruit.com/product/2842)
- [NodeMCU](http://geni.us/4pVoT)
- [P-Channel MOSFET] ()
- [330Ohm Resisters] ()
- [Aluminum Mounting Channel/Diffuser](http://geni.us/JBDhv7)
- [5V 15amp Power Supply](http://geni.us/8rKC)
- [Strip Connector](http://geni.us/OL7tHv)
- [Logic Level Shifter](http://geni.us/4hJAyy)
- [20 Gauge Wire](http://geni.us/2MBYAXF)
- [Cable Chase](http://geni.us/lFqD)
- [Project Box](http://geni.us/kZRgaj)
- [Header Wires](http://geni.us/GniKAX)
- [Power Jacks](http://geni.us/7Ywdut)


#### Wiring Diagram
![alt text](https://github.com/DotNetDann/ESP8266-MQTT-JSON-SK6812RGBW-HomeAssistant/blob/master/Wiring%20Diagram.png?raw=true "Wiring Diagram")

