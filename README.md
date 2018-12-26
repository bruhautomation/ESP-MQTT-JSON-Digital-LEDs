# ESP MQTT JSON Digital LEDs

This is a modified version of https://github.com/bkpsu/ESP-MQTT-JSON-Digital-LEDs which is a modified version of https://github.com/bruhautomation/ESP-MQTT-JSON-Digital-LEDs

The code covered in this repository utilizes [Home Assistant's MQTT Light Component](https://home-assistant.io/components/light.mqtt_json/) and an ESP8266 microcontroller. 

I would suggest using the same versions of libraries to minimize potential isues, PubSubClient (2.7.0), ArduinoJSON (5.12.0) and FastLED (3.2.1)

#### Supported Features Include
- RGB Color Selection
- Brightness 
- Flash
- Fade
- Transitions
- Effects with Animation Speed
- Over-the-Air (OTA) Upload from the ArduinoIDE!
- Music Visualizer (Left to Right and Middle)
- DrZZs Effects
- bkpsu Effects

#### Music Visualizer
This requires some sort of sound device, Microphone or a speaker output from an amp. for my purposes i spliced in to the speaker output of a rear speaker and connected 1 end to my amp and the other to the NodeMCU. The pins that are used for this code are A0 for the possitive (red speaker wire) and GND for the GND (black speaker wire). This works perfectly without requiring any extra hardware. it also doesn't get ruined by voice as it's using a direct sound output.

Some of the effects incorporate the currrently selected color (sinelon, confetti, juggle, etc) while other effects use pre-defined colors. You can also select custom transition speeds between colors. The transition variable in Home Assistant (HA) also functions to control the animation speed of the currently running animation. The input_slider and automation in the HA configuration example allow you to easily set a transition speed from HA's user interface without needing to use the Services tool. 

The default speed for the effects is hard coded and is set when the light is first turned on. When changing between effects, the previously used transition speed will take over. If the effects don't look great, play around with the slider to adjust the transition speed (AKA the effect's animation speed). 

#### OTA Uploading
This code also supports remote uploading to the ESP8266 using Arduino's OTA library. To utilize this, you'll need to first upload the sketch using the traditional USB method. However, if you need to update your code after that, your WIFI-connected ESP chip should show up as an option under Tools -> Port -> Porch at your.ip.address.xxx. More information on OTA uploading can be found [here](http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html). Note: You cannot access the serial monitor over WIFI at this point.  

#### Demo Video

#### Tutorial Video

#### Parts List

#### Wiring Diagram
![alt text](https://github.com/bruhautomation/ESP-MQTT-Digital-LEDs/blob/master/ESP%20MQTT%20Digital%20LEDs%20Wiring%20Diagram.png?raw=true "Wiring Diagram")
Note this modified code uses Pin D3 instead of Pin D5

#### Home Assistant Configuration YAML
````
light:
  - platform: mqtt
    schema: json
    name: "RGB Light Strip"
    state_topic: "home/RGBStrip1"
    command_topic: "home/RGBStrip1/set"
    effect: true
    effect_list:
      - solid    
      - musicVisualizer (965)
      - musicVisualizer (L2R)
      - musicVisualizer (Middle)
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
      - sinelon    
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
