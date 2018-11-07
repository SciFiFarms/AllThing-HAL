// Pair a sensor to a node to make sending statuses a breeze. 
#ifndef SENSORNODEPAIR_H
#define SENSORNODEPAIR_H

struct SensorNodePair {
  Adafruit_Sensor *sensor;
  SensorNode *node;
  public:
    SensorNodePair(Adafruit_Sensor* s, SensorNode* n):
      sensor(s),
      node(n)
    {}
    SensorNodePair()
    {}
};
#endif