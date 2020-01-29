//MQTT things
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
// The secrets on the file arduino_secrets.h
#include "arduino_secrets.h"

#define MQTTtime 60000 //mandamos datos a MQTT cada 60s

// Create a WiFiClient class to connect to the MQTT server.
WiFiClient client;

// we would try WiFiFlientSecure for SSL in a future
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_PW);

/***********************configurar los feeds************************************/

//temperatura caldera
Adafruit_MQTT_Publish calTC = Adafruit_MQTT_Publish(&mqtt, "/casa/caldera/tempC"); 
//temperatura entrada de agua
Adafruit_MQTT_Publish calTI = Adafruit_MQTT_Publish(&mqtt, "/casa/caldera/tempIn");
//temperatura del ACS
Adafruit_MQTT_Publish calTA = Adafruit_MQTT_Publish(&mqtt, "/casa/caldera/tempACS");
//estado del quemador
Adafruit_MQTT_Publish calQ = Adafruit_MQTT_Publish(&mqtt, "/casa/caldera/State/quemador");
//estado de la válvula de 3 vías
Adafruit_MQTT_Publish calV = Adafruit_MQTT_Publish(&mqtt, "/casa/caldera/State/V3v");
//estado de la bomba recirculación
Adafruit_MQTT_Publish calB = Adafruit_MQTT_Publish(&mqtt, "/casa/caldera/State/bomba");

//recibir comando del termostato
Adafruit_MQTT_Subscribe set = Adafruit_MQTT_Subscribe(&mqtt, "/casa/caldera/set");


void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
