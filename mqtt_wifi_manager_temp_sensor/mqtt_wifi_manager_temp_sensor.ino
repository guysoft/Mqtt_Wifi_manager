#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <PubSubClient.h>
#include <DoubleResetDetect.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager

/********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2 
// On Wimos with the dataptor it should sit on D6 on the scew terminal or D4 on the board
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 

int sensor_pin = A0;
//flag for saving data
bool shouldSaveConfig = false;


// maximum number of seconds between resets that
// counts as a double reset 
#define DRD_TIMEOUT 2.0

// address to the block in the RTC user memory
// change it if it collides with another usage 
// of the address block
#define DRD_ADDRESS 0x00

DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50], mqtt_topic[40], mqtt_server[40];

float output_value;

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}



void setup() {
    int sensor_pin = A0;
    Serial.begin(115200); 
     sensors.begin(); 

    //clean FS, for testing
    //SPIFFS.format();

    //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_topic, json["mqtt_topic"]);

        } else {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  } else {
    Serial.println("failed to mount FS, formatting");
    SPIFFS.format();
  }
  //end read

  WiFiManagerParameter custom_mqtt_server("server", "mqtt server (eg test.mosquitto.org)", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_topic("topic", "mqtt topic (eg esp/moisture1)", mqtt_topic, 40);

    

    // Value from the sensor
    float output_value;
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

    //set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_topic);

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //reset settings - for testing
  //wifiManager.resetSettings();

    if (drd.detect())
    {
        Serial.println("** Double reset boot **");
        wifiManager.resetSettings();
    }

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

    //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_topic, custom_mqtt_topic.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_topic"] = mqtt_topic;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
  }


  client.setServer(mqtt_server, 1883);

}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection... to server:  ");
    Serial.print(mqtt_server);
    Serial.print(" topic: ");
    Serial.print(mqtt_topic);
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  char buffer_str[7];
  
  // Send to mqtt server
  Serial.print(" Requesting temperatures..."); 
  sensors.requestTemperatures(); // Send the command to get temperature readings 
  output_value = sensors.getTempCByIndex(0);
  Serial.print("Pushing result: ");
  Serial.println(output_value);
  dtostrf(output_value, 4, 6, buffer_str);  //4 is mininum width, 6 is precision
  //itoa(output_value, buffer_str, 10);
  Serial.println(buffer_str);
  
  client.publish(mqtt_topic, buffer_str);
  Serial.println("DONE"); 
/********************************************************************/
/*
 Serial.print("Temperature is: "); 
 Serial.print(sensors.getTempCByIndex(0)); // Why "byIndex"?  
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire 
   */
  delay(1000);

}
