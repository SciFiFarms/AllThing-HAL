#pragma once

#include <Adafruit_TSL2561_U.h> 
#include <Homie.h>
#include <Wire.h>
#include "Constants.hpp"
#include <ArduinoJson.h>

class TslSensor {
public:
  TslSensor(std::vector<SensorNode*> &sensors, const char* id, uint8_t scl, uint8_t sda);
};

/**
  @param sensors
  @param id for the node.
  @param pin that the sensor is plugged in to. 
 */
TslSensor::TslSensor(std::vector<SensorNode*> &sensors, const char* id, uint8_t scl, uint8_t sda) {
    DynamicJsonBuffer parseJsonBuffer(JSON_OBJECT_SIZE(15));
    Adafruit_TSL2561_Unified* tsl = new Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 3); 
    TwoWire *wire = new TwoWire();
    wire->begin(scl,sda);
    //pinMode(pin, OUTPUT);
    tsl->begin(wire);
    
    Homie.getLogger() << id << " on scl: " << scl << " and sda: " << sda << endl;
     
    sensors.push_back(new SensorNode(id, "Lux", tsl));
}
