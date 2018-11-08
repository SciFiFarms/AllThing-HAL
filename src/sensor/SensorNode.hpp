#pragma once

#include <Homie.h>
#include "Constants.hpp"
#include <Adafruit_Sensor.h>

class SensorNode : public HomieNode {
public:
  SensorNode(const char* id, const char* unit, Adafruit_Sensor* sensor);
  void setValue(const String& value);
  bool getEvent(sensors_event_t* event);
  void getSensor(sensor_t* sensor);
  void readSensorAndReport();

protected:
  Adafruit_Sensor* _sensor;
  const char* _unit;
  void onReadyToOperate();
};

SensorNode::SensorNode(const char* id, const char* unit, Adafruit_Sensor* sensor) : HomieNode(id, TYPE_SENSOR), _unit(unit), _sensor(sensor) {
  advertise(PROP_VALUE);
  advertise(PROP_VALUE_UNIT);
}

void SensorNode::onReadyToOperate() { setProperty(PROP_VALUE_UNIT).send(_unit); }

void SensorNode::setValue(const String& value) { setProperty(PROP_VALUE).send(value); }

bool SensorNode::getEvent(sensors_event_t* event) { return _sensor->getEvent(event); };
void SensorNode::getSensor(sensor_t* sensor) {return _sensor->getSensor(sensor); };
void SensorNode::readSensorAndReport() {
  sensors_event_t event;
  sensor_t sensor;
  float reading;
  this->getSensor(&sensor);
  this->getEvent(&event);
  switch(sensor.type)
  {
    case SENSOR_TYPE_RELATIVE_HUMIDITY:
      reading = event.relative_humidity;
      this->setValue(String(reading));
      Homie.getLogger() << F("Humidity   : ") << reading << " %" << endl;
      break;
    case SENSOR_TYPE_AMBIENT_TEMPERATURE:
      reading = event.temperature;
      this->setValue(String(reading));
      Homie.getLogger() << F("Temperature: ") << reading << " °C" << endl;
      break;
  }
}
