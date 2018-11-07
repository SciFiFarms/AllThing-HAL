#pragma once

#include <DHT.h>
#include <DHT_U.h>
#include <Homie.h>
#include <Vector.h>
#include "Constants.hpp"
#include "sensor/SensorNodePair.hpp"

class DhtTemperatureSensor : public Adafruit_Sensor {
  public:
    DhtTemperatureSensor(DHT_Unified* parent):
      _dht_parent(parent)
    {}
    bool getEvent(sensors_event_t* event) {
      return _dht_parent->temperature().getEvent(event);
    }

    void getSensor(sensor_t* sensor) {
      _dht_parent->temperature().getSensor(sensor);
    }
  private:
    DHT_Unified* _dht_parent;
};

class DhtHumiditySensor : public Adafruit_Sensor {
  public:
    DhtHumiditySensor(DHT_Unified* parent):
      _dht_parent(parent)
    {}
    bool getEvent(sensors_event_t* event) {
      return _dht_parent->humidity().getEvent(event);
    }

    void getSensor(sensor_t* sensor) {
      _dht_parent->humidity().getSensor(sensor);
    }
  private:
    DHT_Unified* _dht_parent;
};

class DhtSensor {
public:
  DhtSensor(std::vector<SensorNodePair> &sensors, uint8_t model, const char* temperatureId, const char* humidityId, uint8_t pin);
};

/**
  @param sensors
  @param model of the DHT sensor (DHT11, DHT21, DHT22)
    Can be DHT11, DHT22 (AM2302), DHT21 (AM2301)
  @param temperature_id for the temperature node.
  @param _id for the temperature node.
  @param pin that the sensor is plugged in to. 
 */
DhtSensor::DhtSensor(std::vector<SensorNodePair> &sensors, uint8_t model, const char* temperatureId, const char* humidityId, uint8_t pin) {
    // Initialize DHT sensor
    DHT_Unified *dhtu = new DHT_Unified(pin, model); 
    pinMode(pin, OUTPUT);
    dhtu->begin();
    
    Homie.getLogger() << temperatureId << " and " << humidityId << " on pin " << pin << endl;
    SensorNode *temperatureNode = new SensorNode(temperatureId, UNIT_TEMPERATURE);
    SensorNode *humidityNode = new SensorNode(humidityId, UNIT_PERCENT);
     
    sensors.push_back(SensorNodePair(new DhtTemperatureSensor(dhtu), temperatureNode));
    sensors.push_back(SensorNodePair(new DhtHumiditySensor(dhtu), humidityNode));
}
