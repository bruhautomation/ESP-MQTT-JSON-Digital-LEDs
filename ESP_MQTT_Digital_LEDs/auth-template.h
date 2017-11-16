/* Add your keys & rename this file to auth.h */

#ifndef _AUTH_DETAILS
#define _AUTH_DETAILS

#define SENSORNAME "porch" //change this to whatever you want to call your device

#define NUM_LEDS    128 // 2.14m x 60 per meter
#define DATA_PIN_LEDS    15  // D8 on ESP8266
#define BRIGHTNESS  0.6  // 60% - Brightness Percentage (This is used to limit the current from the power supply) - 2.4A with 128 leds on RGB White (60ma)

#define DATA_PIN_RELAY    13  // D7 on ESP8266

#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"

#define MQTT_SERVER "mqtt server ip"
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_PORT 1883

#define MQTT_STATE_TOPIC "led/porch"
#define MQTT_SET_TOPIC "led/porch/set"

#define OTApassword "ota password" //the password you will need to enter to upload remotely via the ArduinoIDE
#define OTAport 8266

#endif
