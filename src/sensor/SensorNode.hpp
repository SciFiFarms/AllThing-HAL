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
    case SENSOR_TYPE_LIGHT:
      reading = event.light;
      this->setValue(String(reading));
      Homie.getLogger() << F("Light      : ") << reading << " Lux" << endl;
      break;
    case SENSOR_TYPE_PRESSURE:
      reading = event.pressure;
      this->setValue(String(reading));
      Homie.getLogger() << F("Pressure   : ") << reading << " hPa" << endl;
      break;
    case SENSOR_TYPE_VOLTAGE:
      reading = event.voltage;
      this->setValue(String(reading));
      Homie.getLogger() << F("Voltage    : ") << reading << " V" << endl;
      break;
    case SENSOR_TYPE_CURRENT:
      reading = event.current;
      this->setValue(String(reading));
      Homie.getLogger() << F("Current    : ") << reading << " mA" << endl;
      break;
    default:
      Homie.getLogger() << F("Could not get sensor type. ") << endl;

    // TODO: These are not floats. They should get their own readingSENSOR_TYPES.
    /**
    case SENSOR_TYPE_ACCELEROMETER:
      reading = event.acceleration;
      this->setValue(String(reading));
      Homie.getLogger() << F("Acceleration: ") << reading << "m/s^2" << endl;
      break;
    case SENSOR_TYPE_MAGNETIC_FIELD:
      reading = event.magnetic;
      this->setValue(String(reading));
      Homie.getLogger() << F("Magnetic: ") << reading << "uT" << endl;
      break;
    case SENSOR_TYPE_ORIENTATION:
      reading = event.orientation;
      this->setValue(String(reading));
      Homie.getLogger() << F("Orientation: ") << reading << "degrees" << endl;
      break;
    case SENSOR_TYPE_GYROSCOPE:
      reading = event.gyro;
      this->setValue(String(reading));
      Homie.getLogger() << F("Gyro: ") << reading << "rads/s" << endl;
      break;
    case SENSOR_TYPE_COLOR:
      reading = event.color;
      this->setValue(String(reading));
      Homie.getLogger() << F("Color: ") << reading << "RGB" << endl;
      break;
    case SENSOR_TYPE_GRAVITY:
      reading = event.acceleration;
      this->setValue(String(reading));
      Homie.getLogger() << F("Gravity: ") << reading << "m/s^2" << endl;
      break;
    case SENSOR_TYPE_LINEAR_ACCELERATION:
      reading = event.acceleration;
      this->setValue(String(reading));
      Homie.getLogger() << F("Acceleration not including gravity: ") << reading << "m/s^2" << endl;
      break;
    */

    // I'm not sure what to do with a proximity sensor. It wasn't in the 
    // struct union: https://github.com/adafruit/Adafruit_Sensor/blob/master/Adafruit_Sensor.h
    /*
    case SENSOR_TYPE_PROXIMITY:
      reading = event.temperature;
      this->setValue(String(reading));
      Homie.getLogger() << F("Temperature: ") << reading << " °C" << endl;
      break;
    */
    // I'm not sure what to do with a rotation vector sensor. It wasn't in the 
    // struct union: https://github.com/adafruit/Adafruit_Sensor/blob/master/Adafruit_Sensor.h
    /*
    case SENSOR_TYPE_ROTATION_VECTOR:
      reading = event.temperature;
      this->setValue(String(reading));
      Homie.getLogger() << F("Temperature: ") << reading << " °C" << endl;
      break;
    */
  }
}
