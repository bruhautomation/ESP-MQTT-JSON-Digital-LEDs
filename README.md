# ESP MQTT JSON Digital LEDs

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

#### Optional Relay

Addressable LEDs are different to a normal string. Every pixel ALWAYS has power as it is awaiting commands. So even while the string is off, a few watts will be waisted (1-3 based upon your string length) 
If you connect up an optional MOSFET or relay, when the LED’s are set to off the LED strip will be disconnected saving power.


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
![alt text](https://github.com/bruhautomation/ESP-MQTT-Digital-LEDs/blob/master/ESP%20MQTT%20Digital%20LEDs%20Wiring%20Diagram.png?raw=true "Wiring Diagram")


#### Home Assistant Service Examples
Besides using the card in Home Assistant's user interface, you can also use the Services tool to control the light using the light.turn_on and light.turn_off services. This will let you play with the parameters you can call later in automations or scripts. 

Fade the Light On Over 5 Seconds - light.turn_on
```
{"entity_id":"light.porch_strip",
"brightness":150,
"color_name":"blue",
"transition":"5"
}
```

Flash The Light - light.turn_on
```
{"entity_id":"light.porch_strip",
"color_name":"green",
"brightness":255,
"flash":"short"
}
```

Call Rainbow Effect with Slow Animation Speed - light.turn_on
```
{"entity_id":"light.porch_strip",
"transition":"50",
"brightness":255,
"effect":"rainbow"
}
```

Fade the Light Off Over 5 Seconds - light.turn_off
```
{"entity_id":"light.porch_strip",
"transition":"50"
}
```
