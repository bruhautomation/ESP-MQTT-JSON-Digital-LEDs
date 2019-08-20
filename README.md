This is a modified version of https://github.com/bkpsu/ESP-MQTT-JSON-Digital-LEDs which is a modified version of https://github.com/bruhautomation/ESP-MQTT-JSON-Digital-LEDs

The code covered in this repository utilizes [Home Assistant's MQTT Light Component](https://home-assistant.io/components/light.mqtt_json/) and an ESP8266 microcontroller. 

I would suggest using the same versions of libraries to minimize potential isues, PubSubClient (2.7.0), ArduinoJSON (5.12.0) and FastLED (3.2.1)

#### Supported Features Include
- RGB Color Selection
- Brightness 
- Flash
- Fade
- Transitions
- Effects with Animation Speed controlled with White_Value for ease of use
- Over-the-Air (OTA) Upload from the ArduinoIDE!
- Music Visualizer (Various effects), sensitivity controlled by White_Value
- DrZZs Effects
- bkpsu Effects

#### Music Visualizer
This requires some sort of sound device, Microphone or a speaker output from an amp. for my purposes i spliced in to the speaker output of a rear speaker and connected 1 end to my amp and the other to the NodeMCU. The pins that are used for this code are A0 for the possitive (red speaker wire) and GND for the GND (black speaker wire). This works perfectly without requiring any extra hardware. it also doesn't get ruined by voice as it's using a direct sound output.

Some of the effects incorporate the currrently selected color (sinelon, confetti, juggle, etc) while other effects use pre-defined colors. You can also select custom transition speeds between colors. The transition variable in Home Assistant (HA) also functions to control the animation speed of the currently running animation. The input_slider and automation in the HA configuration example allow you to easily set a transition speed from HA's user interface without needing to use the Services tool. 

The default speed for the effects is hard coded and is set when the light is first turned on. When changing between effects, the previously used transition speed will take over. If the effects don't look great, play around with the slider to adjust the transition speed (AKA the effect's animation speed). 

#### OTA Uploading
This code also supports remote uploading to the ESP8266 using Arduino's OTA library. To utilize this, you'll need to first upload the sketch using the traditional USB method. However, if you need to update your code after that, your WIFI-connected ESP chip should show up as an option under Tools -> Port -> Porch at your.ip.address.xxx. More information on OTA uploading can be found [here](http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html). Note: You cannot access the serial monitor over WIFI at this point.  

#### Demo Video
[![Demo Video](http://i.imgur.com/cpW2JAX.png)](https://www.youtube.com/watch?v=DQZ4x6Z3678 "Demo - RGB Digital LED Strip controlled using ESP, MQTT, and Home Assistant")

#### Tutorial Video
[![Tutorial Video](http://i.imgur.com/9UMl8Xo.jpg)](https://www.youtube.com/watch?v=9KI36GTgwuQ "The BEST Digital LED Strip Light Tutorial - DIY, WIFI-Controllable via ESP, MQTT, and Home Assistant")

#### Home Assistant Configuration YAML
````
mqtt:
  broker: your.mqtt.ip.address
  port: 1883
  client_id: home-assistant-1
  username: YOURUSERNAME
  password: YOURPASSWORD

light:
    - platform: mqtt
    schema: json
    name: "RGB Light Strip 1"
    state_topic: "home/RGBStrip1"
    command_topic: "home/RGBStrip1/set"
    rgb: true
    availability_topic: "home/RGBStrip1/LWT"
    white_value: true
    effect: true
    effect_list:
      - solid    
      - Music - L2R
      - Music - Middle
      - Music - Fma965
      - bpm
      - candy cane  
      - confetti  
      - cyclon rainbow  
      - dots  
      - fire  
      - glitter  
      - juggle  
      - lightning
      - noise  
      - police all  
      - police one  
      - rainbow  
      - rainbow with glitter  
      - ripple      
      - twinkle
      - sinelon
      - sine hue
      - full hue
      - breathe
      - hue breathe
      - Christmas
      - christmas alternate
      - random stars
      - St Patty
      - Valentine
      - Turkey Day
      - Thanksgiving
      - USA
      - Independence
      - Halloween
      - Go Lions
      - Hail
      - Touchdown
      - Punkin
      - Lovey Day
      - Holly Jolly      
    brightness: true
    optimistic: false
    qos: 0
````

#### Example MQTT Payload
````
  {
    "brightness": 120,
    "color": {
      "r": 255,
      "g": 100,
      "b": 100
    },
    "flash": 2,
    "transition": 5,
    "state": "ON"
  }
````

#### Parts List
- [Digital RGB Leds](http://geni.us/8mBml)
- [NodeMCU](http://geni.us/4pVoT)
- [Aluminum Mounting Channel/Diffuser](http://geni.us/JBDhv7)
- [12v to 5v Step Down](http://geni.us/PghhV9)
- [12V 15amp Power Supply](http://geni.us/8rKC)
- [Strip Connector](http://geni.us/OL7tHv)
- [Logic Level Shifter](http://geni.us/4hJAyy)
- [20 Gauge Wire](http://geni.us/2MBYAXF)
- [Cable Chase](http://geni.us/lFqD)
- [Project Box](http://geni.us/kZRgaj)
- [Header Wires](http://geni.us/GniKAX)
- [Power Jacks](http://geni.us/7Ywdut)


#### Wiring Diagram
PLEASE NOTE I USE PIN D3 IN MY CODE, NOT D5
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
"white_value":"50",
"brightness":255,
"effect":"rainbow"
}
```

Fade the Light Off Over 5 Seconds - light.turn_off
```
{"entity_id":"light.porch_strip",
"white_value":"50"
}
```
