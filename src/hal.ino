#include <Homie.h>
//#include <GDBStub.h>
//#include <HomieSetting.h>
//#include <SensorNode.hpp>
#include "../SensorNode.hpp"
#include <Adafruit_Sensor.h>

// TODO: Refactor MEASURE_INTERVAL to be a setting that is retrieved.
const int MEASURE_INTERVAL = 1; // How often to poll DHT22 for temperature and humidity
// TODO: Refactor lastMeasureSent so that it is static rather than a global. 
unsigned long lastMeasureSent = 0;

// The DHT Sensors
#include "DHT.h"
// Can be DHT11, DHT22 (AM2302), DHT21 (AM2301)
DHT *dht11; // Initialize DHT sensor for normal 16mhz Arduino
  HomieSetting<long> dht11Setting("dht11_pin", "Which pin to use.");  // id, description
  HomieNode *dht11_temperatureNode;
  HomieNode *dht11_humidityNode;
DHT *dht21; // Initialize DHT sensor for normal 16mhz Arduino
  HomieSetting<long> dht21Setting("dht21_pin", "Which pin to use.");  // id, description
  HomieNode *dht21_temperatureNode;
  HomieNode *dht21_humidityNode;
DHT *dht22; // Initialize DHT sensor for normal 16mhz Arduino
  HomieSetting<long> dht22Setting("dht22_pin", "Which pin to use.");  // id, description
  SensorNode *dht22_temperatureNode;
  SensorNode *dht22_humidityNode;

// List of supported sensors and their IDs.
#define SENSOR_DHT11_ID = 1
#define SENSOR_DHT21_ID = 2 
#define SENSOR_DHT22_ID = 3

/*Sensors nodes[] = {
  {DHT22_TEMPERATURE_ID, HomieNode temperatureNode("temperature", "temperature")},
  {DHT22_HUMIDITY_ID, HomieNode humidityNode("humidity", "humidity")}
};
*/


void loopHandler() {
  if (millis() - lastMeasureSent >= MEASURE_INTERVAL * 1000UL || lastMeasureSent == 0) {
    if(dht11Setting.wasProvided())
    {
      float temperature = dht11->readTemperature(); // Read temperature as Celsius
      float humidity = dht11->readHumidity(); // Read humidity as relative [0-100]%
      if (isnan(temperature) || isnan(humidity)) {
        Homie.getLogger() << F("Failed to read from DHT sensor!");
      } else {
        Homie.getLogger() << F("Temperature: ") << temperature << " °C" << endl;
        Homie.getLogger() << F("Humidity   : ") << humidity << " %" << endl;
        dht11_temperatureNode->setProperty("degrees").send(String(temperature));
        dht11_humidityNode->setProperty("relative").send(String(humidity));
      }
      lastMeasureSent = millis();
    }
    if(dht21Setting.wasProvided())
    {
      float temperature = dht21->readTemperature(); // Read temperature as Celsius
      float humidity = dht21->readHumidity(); // Read humidity as relative [0-100]%
      if (isnan(temperature) || isnan(humidity)) {
        Homie.getLogger() << F("Failed to read from DHT sensor!");
      } else {
        Homie.getLogger() << F("Temperature: ") << temperature << " °C" << endl;
        Homie.getLogger() << F("Humidity   : ") << humidity << " %" << endl;
        dht21_temperatureNode->setProperty("degrees").send(String(temperature));
        dht21_humidityNode->setProperty("relative").send(String(humidity));
      }
      lastMeasureSent = millis();
    }
    if(dht22Setting.wasProvided())
    {
      float temperature = dht22->readTemperature(); // Read temperature as Celsius
      float humidity = dht22->readHumidity(); // Read humidity as relative [0-100]%
      if (isnan(temperature) || isnan(humidity)) {
        Homie.getLogger() << F("Failed to read from DHT sensor!");
      } else {
        Homie.getLogger() << F("Temperature: ") << temperature << " °C" << endl;
        Homie.getLogger() << F("Humidity   : ") << humidity << " %" << endl;
        dht22_temperatureNode->setProperty("degrees").send(String(temperature));
        dht22_humidityNode->setProperty("relative").send(String(humidity));
      }
      lastMeasureSent = millis();
    }
  }
}

void setupHandler() {
  // TODO: See if I can move these into setup(). Or move more of setup here.
  if(dht11Setting.wasProvided())
  {
    // Nodes part
    dht11_temperatureNode->advertise("unit");
    dht11_temperatureNode->advertise("degrees");
    dht11_temperatureNode->setProperty("unit").send("c");
    dht11_humidityNode->advertise("unit");
    dht11_humidityNode->advertise("relative");
    dht11_humidityNode->setProperty("unit").send("%");

    // Hardware part
    dht11 = new DHT(dht11Setting.get(), DHT11); // Initialize DHT sensor for normal 16mhz Arduino
    pinMode(dht11Setting.get(), OUTPUT);
    dht11->begin();
    Homie.getLogger() << "DHT11 on pin " << dht11Setting.get() << endl;
  }
  if(dht21Setting.wasProvided())
  {
    // Nodes part
    dht21_temperatureNode->advertise("unit");
    dht21_temperatureNode->advertise("degrees");
    dht21_temperatureNode->setProperty("unit").send("c");
    dht21_humidityNode->advertise("unit");
    dht21_humidityNode->advertise("relative");
    dht21_humidityNode->setProperty("unit").send("%");

    // Hardware part
    dht21 = new DHT(dht21Setting.get(), DHT21); // Initialize DHT sensor for normal 16mhz Arduino
    pinMode(dht21Setting.get(), OUTPUT);
    dht21->begin();
    Homie.getLogger() << "DHT21 on pin " << dht21Setting.get() << endl;
  }
  if(dht22Setting.wasProvided())
  {
    // Next step: Figure out if these can live here or if they belong in setup().
    // Nodes part
    dht22_temperatureNode->setProperty("unit").send("c");
    dht22_humidityNode->setProperty("unit").send("%");

    // Hardware part
    dht22 = new DHT(dht22Setting.get(), DHT22); // Initialize DHT sensor for normal 16mhz Arduino
    pinMode(dht22Setting.get(), OUTPUT);
    dht22->begin();
    Homie.getLogger() << "DHT22 on pin " << dht22Setting.get() << endl;
  }
}

void setup() {
  Serial.begin(115200); // Required to enable serial output
  Homie.setLedPin(15, HIGH);
  //Homie.enableBuiltInLedIndicator(false);
  //dht.setup(2); // data pin 2
  //Serial << "HELLO" << endl;

  Homie_setFirmware("hal", "1.0.0");
  Homie.setSetupFunction(setupHandler);
  Homie.setLoopFunction(loopHandler);

  // Load sensor settings
  dht11Setting.setDefaultValue(0).setValidator([] (long candidate) {
    return (candidate >= 0) && (candidate <= 100);
  }); 
  dht21Setting.setDefaultValue(0).setValidator([] (long candidate) {
    return (candidate >= 0) && (candidate <= 100);
  }); 
  dht22Setting.setDefaultValue(0).setValidator([] (long candidate) {
    return (candidate >= 0) && (candidate <= 100);
  }); 

  // TODO: I've commented out these if statements because as of homie 2.0.0,
  // the setting objects aren't setup before Homie.setup() is called. In 2.1, 
  // it sounds like I'll be able to call Homie.loadSettings() or something like
  // that to initialise the setting objects.
  //if(dht11Setting.wasProvided())
  //{
    dht11_temperatureNode = new HomieNode("dht11_temperature", "temperature");
    dht11_humidityNode = new HomieNode("dht11_humidity", "humidity");
  //}
  //if(dht21Setting.wasProvided())
  //{
    dht21_temperatureNode = new HomieNode("dht21_temperature", "temperature");
    dht21_humidityNode = new HomieNode("dht21_humidity", "humidity");
  //}
  //if(dht22Setting.wasProvided())
  //{
    dht22_temperatureNode = new HomieNode("dht22_temperature", "sensor");
    dht22_humidityNode = new HomieNode("dht22_humidity", "sensor");
    dht22_temperatureNode->advertise("unit");
    dht22_temperatureNode->advertise("degrees");
    dht22_humidityNode->advertise("unit");
    dht22_humidityNode->advertise("relative");
  //}

  Homie.setup();
}


void loop() {
  Homie.loop();
}