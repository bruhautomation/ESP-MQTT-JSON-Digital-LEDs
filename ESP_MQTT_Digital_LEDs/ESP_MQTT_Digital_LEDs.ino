/*
  .______   .______    __    __   __    __          ___      __    __  .___________.  ______   .___  ___.      ___   .___________. __    ______   .__   __.
  |   _  \  |   _  \  |  |  |  | |  |  |  |        /   \    |  |  |  | |           | /  __  \  |   \/   |     /   \  |           ||  |  /  __  \  |  \ |  |
  |  |_)  | |  |_)  | |  |  |  | |  |__|  |       /  ^  \   |  |  |  | `---|  |----`|  |  |  | |  \  /  |    /  ^  \ `---|  |----`|  | |  |  |  | |   \|  |
  |   _  <  |      /  |  |  |  | |   __   |      /  /_\  \  |  |  |  |     |  |     |  |  |  | |  |\/|  |   /  /_\  \    |  |     |  | |  |  |  | |  . `  |
  |  |_)  | |  |\  \-.|  `--'  | |  |  |  |     /  _____  \ |  `--'  |     |  |     |  `--'  | |  |  |  |  /  _____  \   |  |     |  | |  `--'  | |  |\   |
  |______/  | _| `.__| \______/  |__|  |__|    /__/     \__\ \______/      |__|      \______/  |__|  |__| /__/     \__\  |__|     |__|  \______/  |__| \__|

  Thanks much to @corbanmailloux for providing a great framework for implementing flash/fade with HomeAssistant https://github.com/corbanmailloux/esp-mqtt-rgb-led
  
  To use this code you will need the following dependancies: 
  
  - Support for the ESP8266 boards. 
        - You can add it to the board manager by going to File -> Preference and pasting http://arduino.esp8266.com/stable/package_esp8266com_index.json into the Additional Board Managers URL field.
        - Next, download the ESP8266 dependancies by going to Tools -> Board -> Board Manager and searching for ESP8266 and installing it.
  
  - You will also need to download the follow libraries by going to Sketch -> Include Libraries -> Manage Libraries
      - Adafruit NeoPixel 
      - PubSubClient
      - ArduinoJSON
*/

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "auth.h"


const char* on_cmd = "ON";
const char* off_cmd = "OFF";
const char* effect = "solid";
String effectString = "solid";
String oldeffectString = "solid";



/****************************************FOR JSON***************************************/
const int BUFFER_SIZE = JSON_OBJECT_SIZE(10);
#define MQTT_MAX_PACKET_SIZE 512

/*********************************** FastLED Defintions ********************************/
byte realRed = 0;
byte realGreen = 0;
byte realBlue = 0;
byte realWhite = 255;

byte red = 0;
byte green = 0;
byte blue = 0;
byte white = 255;
byte brightness = 255;

/******************************** GLOBALS for fade/flash *******************************/
bool stateOn = false;
bool startFade = false;
bool onbeforeflash = false;
unsigned long lastLoop = 0;
int transitionTime = 50; // 1-150
int effectSpeed = 0;
bool inFade = false;
int loopCount = 0;
int stepR, stepG, stepB, stepW;
int redVal, grnVal, bluVal, whiVal;

bool flash = false;
bool startFlash = false;
int flashLength = 0;
unsigned long flashStartTime = 0;
byte flashRed = red;
byte flashGreen = green;
byte flashBlue = blue;
byte flashWhite = white;
byte flashBrightness = brightness;


WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN_LEDS, NEO_GRBW + NEO_KHZ800);


/********************************** START SETUP*****************************************/
void setup() {
  Serial.begin(115200);

  pinMode(DATA_PIN_RELAY, OUTPUT);
  digitalWrite(DATA_PIN_RELAY, HIGH);

  delay(500); // Wait for Leds to init and Cap to charge
  
  // End of trinket special code
  strip.setBrightness(brightness*BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // Standalone startup sequence - Wipe White
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0,0,0,255) );
    strip.show();
    delay(10);
  }

  setup_wifi();

  // OK we are on Wifi so we are no standalone.
  strip.setPixelColor(0, strip.Color(255,0,0,255) ); // Red tinge on first Pixel
  strip.show();
  
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  //OTA SETUP
  ArduinoOTA.setPort(OTAport);
  ArduinoOTA.setHostname(SENSORNAME); // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setPassword((const char *)OTApassword); // No authentication by default

  ArduinoOTA.onStart([]() {
    Serial.println("Starting");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // OK we are connected
  strip.setPixelColor(0, strip.Color(0,255,0,255) ); // Green tinge on first Pixel
  strip.show();
  delay(500); // Wait for Leds to init and Cap to charge
}


/********************************** START SETUP WIFI*****************************************/
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/*
  SAMPLE PAYLOAD:
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
*/



/********************************** START CALLBACK*****************************************/
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  if (!processJson(message)) {
    return;
  }

  if (stateOn) {
    realRed = map(red, 0, 255, 0, brightness);
    realGreen = map(green, 0, 255, 0, brightness);
    realBlue = map(blue, 0, 255, 0, brightness);
    realWhite = map(white, 0, 255, 0, brightness);
  }
  else {
    realRed = 0;
    realGreen = 0;
    realBlue = 0;
    realWhite = 0;
  }

  // Set the relay for the LED strips
  if (!stateOn && digitalRead(DATA_PIN_RELAY)) {
    digitalWrite(DATA_PIN_RELAY, LOW);
    Serial.println("Relay: OFF");
  }
  if (stateOn && !digitalRead(DATA_PIN_RELAY)) {
    digitalWrite(DATA_PIN_RELAY, HIGH);
    delay(500); // Wait for Leds to init and Cap to charge
    Serial.println("Relay: ON");
  }

  Serial.println(effect);

  startFade = true;
  inFade = false; // Kill the current fade

  sendState();
}



/********************************** START PROCESS JSON*****************************************/
bool processJson(char* message) {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return false;
  }

  if (root.containsKey("state")) {
    if (strcmp(root["state"], on_cmd) == 0) {
      stateOn = true;
    }
    else if (strcmp(root["state"], off_cmd) == 0) {
      stateOn = false;
      onbeforeflash = false;
    }
  }

  if (root.containsKey("transition")) {
    transitionTime = root["transition"];
  }
    
  // If "flash" is included, treat RGB and brightness differently
  if (root.containsKey("flash")) {
    flashLength = (int)root["flash"] * 1000;

    oldeffectString = effectString;

    if (root.containsKey("brightness")) {
      flashBrightness = root["brightness"];
    }
    else {
      flashBrightness = brightness;
    }

    if (root.containsKey("color")) {
      flashRed = root["color"]["r"];
      flashGreen = root["color"]["g"];
      flashBlue = root["color"]["b"];
      flashWhite = 0;
    }
    else {
      flashRed = red;
      flashGreen = green;
      flashBlue = blue;      
      flashWhite = white;
    }

    if (root.containsKey("effect")) {
      effect = root["effect"];
      effectString = effect;
      //twinklecounter = 0; //manage twinklecounter
    }

    if (root.containsKey("transition")) {
      transitionTime = root["transition"];
    }
    else if ( effectString == "solid") {
      transitionTime = 50;
    }

    flashRed = map(flashRed, 0, 255, 0, flashBrightness);
    flashGreen = map(flashGreen, 0, 255, 0, flashBrightness);
    flashBlue = map(flashBlue, 0, 255, 0, flashBrightness);
    flashWhite = map(flashWhite, 0, 255, 0, flashBrightness);

    flash = true;
    startFlash = true;
  }
  else { // Not flashing
    flash = false;

    if (stateOn) {   //if the light is turned on and the light isn't flashing
      onbeforeflash = true;
    }

    if (root.containsKey("color")) {
      red = root["color"]["r"];
      green = root["color"]["g"];
      blue = root["color"]["b"];
      white = 0;
    }

    if (root.containsKey("white_value")) {
      red = 0;
      green = 0;
      blue = 0;
      white = root["white_value"];
    }
        
    if (root.containsKey("color_temp")) {
      //temp comes in as mireds, need to convert to kelvin then to RGB
      int color_temp = root["color_temp"];
      int MILLION = 100000;
      unsigned int kelvin  = MILLION / color_temp;
      
      temp2rgb(kelvin);
    }

    if (root.containsKey("brightness")) {
      brightness = root["brightness"];
    }

    if (root.containsKey("effect")) {
      effect = root["effect"];
      effectString = effect;
      //twinklecounter = 0; //manage twinklecounter
    }

    if (root.containsKey("transition")) {
      transitionTime = root["transition"];
    }
    else if ( effectString == "solid") {
      transitionTime = 50;
    }

  }

  return true;
}



/********************************** START SEND STATE*****************************************/
void sendState() {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root["state"] = (stateOn) ? on_cmd : off_cmd;
  JsonObject& color = root.createNestedObject("color");
  color["r"] = red;
  color["g"] = green;
  color["b"] = blue;

  root["white_value"] = white;
  root["brightness"] = brightness;
  root["effect"] = effectString.c_str();

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  client.publish(MQTT_STATE_TOPIC, buffer, true);
}



/********************************** START RECONNECT*****************************************/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(SENSORNAME, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
      client.subscribe(MQTT_SET_TOPIC);
      setColor(0, 0, 0, 0);
      sendState();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



/********************************** START Set Color*****************************************/
void setColor(int inR, int inG, int inB, int inW) {
  
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(inR,inG,inB,inW) );
  }

  strip.show();

  Serial.println("Setting LEDs:");
  Serial.print("r: ");
  Serial.print(inR);
  Serial.print(", g: ");
  Serial.print(inG);
  Serial.print(", b: ");
  Serial.print(inB);
  Serial.print(", w: ");
  Serial.println(inW);
}


#include "NeoPixel_Effects.h"

/********************************** START MAIN LOOP*****************************************/
void loop() {

  if (!client.connected()) {
    reconnect();
  }

  if (WiFi.status() != WL_CONNECTED) {
    delay(1);
    Serial.print("WIFI Disconnected. Attempting reconnection.");
    setup_wifi();
    return;
  }

  client.loop();

  ArduinoOTA.handle();


  //EFFECTS
  if (effectString == "twinkle") {
    //Twinkle(10, 100, false);
    Twinkle(10, (2*transitionTime), false);
  }

  if (effectString == "cylon bounce") {
    //CylonBounce(4, 10, 50);
    CylonBounce(4, transitionTime, 50);
  }

  if (effectString == "fire") {
    //Fire(55,120,15);
    Fire(55,120,(2*transitionTime));
  }

  if (effectString == "fade in out") {
    FadeInOut();
  }

  if (effectString == "strobe") {
    // Slower:
    //Strobe(10, 100);
    // Fast:
    //Strobe(10, 50);
    Strobe(10, transitionTime);
  }

  if (effectString == "theater chase") {
    //theaterChase(50);
    theaterChase(transitionTime);
  }

  if (effectString == "rainbow cycle") {
    //rainbowCycle(20);
    rainbowCycle(transitionTime);
  }

  if (effectString == "color wipe") {
    //colorWipe(50);
    colorWipe(transitionTime);
  }

  if (effectString == "running lights") {
    //RunningLights(50);
    RunningLights(transitionTime);
  }

  if (effectString == "snow sparkle") {
    SnowSparkle(20, random(transitionTime,(10*transitionTime)));
  }

  if (effectString == "sparkle") {
    //Sparkle(0);
    Sparkle(transitionTime);
  }

  if (effectString == "twinkle random") {
    //TwinkleRandom(20, 100, false);
    TwinkleRandom(20, (2*transitionTime), false);
  }





  if (effectString == "bpm") {
  }
  if (effectString == "candy cane") {
  }
  if (effectString == "confetti" ) {
  }
  if (effectString == "dots") {
  }
  if (effectString == "glitter") {
  }
  if (effectString == "juggle" ) {                           // eight colored dots, weaving in and out of sync with each other
  }
  if (effectString == "lightning") {
  }
  if (effectString == "police all") {                 //POLICE LIGHTS (TWO COLOR SOLID)
  }
  if (effectString == "police one") {
  }
  if (effectString == "rainbow") {
  }
  if (effectString == "rainbow with glitter") {               // FastLED's built-in rainbow generator with Glitter
  }





//  EVERY_N_MILLISECONDS(10) {
//
//    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // FOR NOISE ANIMATIon
//    {
//      gHue++;
//    }
//
//    //EFFECT NOISE
//    if (effectString == "noise") {
//    }
//
//    //EFFECT RIPPLE
//    if (effectString == "ripple") {
//    }
//
//  }


//  EVERY_N_SECONDS(5) {
//    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
//  }

  //FLASH AND FADE SUPPORT
  if (flash) {
    if (startFlash) {
      startFlash = false;
      flashStartTime = millis();
    }

    if ((millis() - flashStartTime) <= flashLength) {
      if ((millis() - flashStartTime) % 1000 <= 500) {
        setColor(flashRed, flashGreen, flashBlue, flashWhite);
      }
      else {
        setColor(0, 0, 0, 0);
        // If you'd prefer the flashing to happen "on top of"
        // the current color, uncomment the next line.
        // setColor(realRed, realGreen, realBlue);
      }
    }
    else {
      flash = false;
      effectString = oldeffectString;
      if (onbeforeflash) { //keeps light off after flash if light was originally off
        setColor(realRed, realGreen, realBlue, realWhite);
      }
      else {
        stateOn = false;
        setColor(0, 0, 0, 0);
        sendState();
      }
    }
  }

  if (startFade && effectString == "solid") {
    // If we don't want to fade, skip it.
    if (transitionTime == 0) {
      setColor(realRed, realGreen, realBlue, realWhite);

      redVal = realRed;
      grnVal = realGreen;
      bluVal = realBlue;
      whiVal = realWhite;

      startFade = false;
    }
    else {
      loopCount = 0;
      stepR = calculateStep(redVal, realRed);
      stepG = calculateStep(grnVal, realGreen);
      stepB = calculateStep(bluVal, realBlue);
      stepW = calculateStep(whiVal, realWhite);

      inFade = true;
    }
  }

  if (inFade) {
    startFade = false;
    unsigned long now = millis();
    if (now - lastLoop > transitionTime) {
      if (loopCount <= 1020) {
        lastLoop = now;

        redVal = calculateVal(stepR, redVal, loopCount);
        grnVal = calculateVal(stepG, grnVal, loopCount);
        bluVal = calculateVal(stepB, bluVal, loopCount);
        whiVal = calculateVal(stepW, whiVal, loopCount);

        if (effectString == "solid") {
          setColor(redVal, grnVal, bluVal, whiVal); // Write current values to LED pins
        }
        loopCount++;
      }
      else {
        inFade = false;
      }
    }
  }
}


/**************************** START TRANSITION FADER *****************************************/
// From https://www.arduino.cc/en/Tutorial/ColorCrossfader
/* BELOW THIS LINE IS THE MATH -- YOU SHOULDN'T NEED TO CHANGE THIS FOR THE BASICS
  The program works like this:
  Imagine a crossfade that moves the red LED from 0-10,
    the green from 0-5, and the blue from 10 to 7, in
    ten steps.
    We'd want to count the 10 steps and increase or
    decrease color values in evenly stepped increments.
    Imagine a + indicates raising a value by 1, and a -
    equals lowering it. Our 10 step fade would look like:
    1 2 3 4 5 6 7 8 9 10
  R + + + + + + + + + +
  G   +   +   +   +   +
  B     -     -     -
  The red rises from 0 to 10 in ten steps, the green from
  0-5 in 5 steps, and the blue falls from 10 to 7 in three steps.
  In the real program, the color percentages are converted to
  0-255 values, and there are 1020 steps (255*4).
  To figure out how big a step there should be between one up- or
  down-tick of one of the LED values, we call calculateStep(),
  which calculates the absolute gap between the start and end values,
  and then divides that gap by 1020 to determine the size of the step
  between adjustments in the value.
*/
int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero,
    step = 1020 / step;          //   divide by 1020
  }

  return step;
}
/* The next function is calculateVal. When the loop value, i,
   reaches the step size appropriate for one of the
   colors, it increases or decreases the value of that color by 1.
   (R, G, and B are each calculated separately.)
*/
int calculateVal(int step, int val, int i) {
  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      val += 1;
    }
    else if (step < 0) {         //   ...or decrement it if step is negative
      val -= 1;
    }
  }

  // Defensive driving: make sure val stays in the range 0-255
  if (val > 255) {
    val = 255;
  }
  else if (val < 0) {
    val = 0;
  }

  return val;
}




/********************************** START SHOW LEDS ***********************************************/
//void showleds() {
//
//  delay(1);
//
//  if (stateOn) {
//    strip.setBrightness(brightness*BRIGHTNESS);  //EXECUTE EFFECT COLOR
//    strip.show();;
//    if (transitionTime > 0 && transitionTime < 130) {  //Sets animation speed based on receieved value
//      //strip.delay(1000 / transitionTime);
//      delay(10*transitionTime);
//    }
//  }
//  else if (startFade) {
//    setColor(0, 0, 0, 0);
//    startFade = false;
//  }
//}
void temp2rgb(unsigned int kelvin) {
    int tmp_internal = kelvin / 100.0;
    
    // red 
    if (tmp_internal <= 66) {
        red = 255;
    } else {
        float tmp_red = 329.698727446 * pow(tmp_internal - 60, -0.1332047592);
        if (tmp_red < 0) {
            red = 0;
        } else if (tmp_red > 255) {
            red = 255;
        } else {
            red = tmp_red;
        }
    }
    
    // green
    if (tmp_internal <=66){
        float tmp_green = 99.4708025861 * log(tmp_internal) - 161.1195681661;
        if (tmp_green < 0) {
            green = 0;
        } else if (tmp_green > 255) {
            green = 255;
        } else {
            green = tmp_green;
        }
    } else {
        float tmp_green = 288.1221695283 * pow(tmp_internal - 60, -0.0755148492);
        if (tmp_green < 0) {
            green = 0;
        } else if (tmp_green > 255) {
            green = 255;
        } else {
            green = tmp_green;
        }
    }
    
    // blue
    if (tmp_internal >=66) {
        blue = 255;
    } else if (tmp_internal <= 19) {
        blue = 0;
    } else {
        float tmp_blue = 138.5177312231 * log(tmp_internal - 10) - 305.0447927307;
        if (tmp_blue < 0) {
            blue = 0;
        } else if (tmp_blue > 255) {
            blue = 255;
        } else {
            blue = tmp_blue;
        }
    }
}
