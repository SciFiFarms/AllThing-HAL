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

private:
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