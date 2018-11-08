#include <Adafruit_Sensor.h>
#include <Homie.h>
#include <Vector.h>
#include "sensor/SensorNode.hpp"
#include "sensor/DhtSensor.hpp"

// TODO: Refactor MEASURE_INTERVAL to be a setting that is retrieved.
const int MEASURE_INTERVAL = 1; // How often to poll DHT22 for temperature and humidity
// TODO: Refactor lastMeasureSent so that it is static rather than a global. 
unsigned long lastMeasureSent = 0;

HomieSetting<long> dht11Setting("dht11_pin", "Which pin to use.");  // id, description
HomieSetting<long> dht21Setting("dht21_pin", "Which pin to use.");  // id, description
HomieSetting<long> dht22Setting("dht22_pin", "Which pin to use.");  // id, description

std::vector<SensorNode*> sensors;

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
  Homie.loadSettings();

  // Initialize any configured sensors.
  // TODO: Rather than assign sensors/nodes based on a setting being present, 
  // we should have a series of sensor_[X] settings.
  if(dht11Setting.wasProvided())
    new DhtSensor(sensors, DHT11, "dht11_temperature", "dht11_humidity", dht11Setting.get());
  if(dht21Setting.wasProvided())
    new DhtSensor(sensors, DHT21, "dht21_temperature", "dht21_humidity", dht21Setting.get());
  if(dht22Setting.wasProvided())
    new DhtSensor(sensors, DHT22, "dht22_temperature", "dht22_humidity", dht22Setting.get());

  Homie.setup();
}

void setupHandler() {
}

void loopHandler() {
  if (millis() - lastMeasureSent >= MEASURE_INTERVAL * 1000UL || lastMeasureSent == 0) {
    for(int i = 0; i < sensors.size(); i++)
    {
      sensors[i]->readSensorAndReport();
      lastMeasureSent = millis();
    }
  }
}

void loop() {
  Homie.loop();
}