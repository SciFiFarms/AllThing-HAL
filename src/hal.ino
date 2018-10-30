#include <Homie.h>
#include "sensor/SensorNode.hpp"
#include <Adafruit_Sensor.h>
#include <Vector.h>

// TODO: Refactor MEASURE_INTERVAL to be a setting that is retrieved.
const int MEASURE_INTERVAL = 1; // How often to poll DHT22 for temperature and humidity
// TODO: Refactor lastMeasureSent so that it is static rather than a global. 
unsigned long lastMeasureSent = 0;

// The DHT Sensors
#include <DHT.h>
#include <DHT_U.h>
// Can be DHT11, DHT22 (AM2302), DHT21 (AM2301)
DHT_Unified *dhtu11; // Initialize DHT sensor for normal 16mhz Arduino
  HomieSetting<long> dht11Setting("dht11_pin", "Which pin to use.");  // id, description
  HomieNode *dht11_temperatureNode;
  HomieNode *dht11_humidityNode;
DHT_Unified *dhtu21; // Initialize DHT sensor for normal 16mhz Arduino
  HomieSetting<long> dht21Setting("dht21_pin", "Which pin to use.");  // id, description
  HomieNode *dht21_temperatureNode;
  HomieNode *dht21_humidityNode;
DHT_Unified *dhtu22; // Initialize DHT sensor for normal 16mhz Arduino
  HomieSetting<long> dht22Setting("dht22_pin", "Which pin to use.");  // id, description
  SensorNode *dht22_temperatureNode;
  SensorNode *dht22_humidityNode;

// Pair a sensor to a node to make sending statuses a breeze. 
struct SensorNodePair {
  Adafruit_Sensor *sensor;
  HomieNode *node;
  public:
    SensorNodePair(Adafruit_Sensor* s, HomieNode* n):
      sensor(s),
      node(n)
    {}
    SensorNodePair()
    {}
};
// Structures to store the sensors in.
Vector<SensorNodePair> sensors;
SensorNodePair storage_array[10];

class DHT_Temperature : public Adafruit_Sensor {
  public:
    DHT_Temperature(DHT_Unified* parent):
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

class DHT_Humidity : public Adafruit_Sensor {
  public:
    DHT_Humidity(DHT_Unified* parent):
      _dht_parent(parent)
    {}
    //bool getEvent(sensors_event_t* event);
    bool getEvent(sensors_event_t* event) {
      return _dht_parent->humidity().getEvent(event);
    }

    void getSensor(sensor_t* sensor) {
      _dht_parent->humidity().getSensor(sensor);
    }
  private:
    DHT_Unified* _dht_parent;
};

void loopHandler() {
  if (millis() - lastMeasureSent >= MEASURE_INTERVAL * 1000UL || lastMeasureSent == 0) {
    sensors_event_t event;
    sensor_t sensor;
    float reading;

    for(int i = 0; i < sensors.size(); i++)
    {
      sensors[i].sensor->getSensor(&sensor);
      sensors[i].sensor->getEvent(&event);
      switch(sensor.type)
    {
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
          // TODO: Perhaps I could add a call in my local Adafruit_Senor class that has 
          // this renamed to something more generic so that I can get
          // numeric_sensors, and have it get and set "value".  
          reading = event.relative_humidity;
          sensors[i].node->setProperty("relative").send(String(reading));
          Homie.getLogger() << F("Humidity   : ") << reading << " %" << endl;
          break;
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
          reading = event.temperature;
          sensors[i].node->setProperty("degrees").send(String(reading));
          Homie.getLogger() << F("Temperature: ") << reading << " °C" << endl;
          break;
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
    dht11_temperatureNode->setProperty("unit").send("c");
    dht11_humidityNode->setProperty("unit").send("%");

    // Hardware part
    dhtu11 = new DHT_Unified(dht11Setting.get(), DHT11); // Initialize DHT sensor for normal 16mhz Arduino
    pinMode(dht11Setting.get(), OUTPUT);
    dhtu11->begin();
    Homie.getLogger() << "DHT11 on pin " << dht11Setting.get() << endl;
  }
  if(dht21Setting.wasProvided())
  {
    // Nodes part
    dht21_temperatureNode->setProperty("unit").send("c");
    dht21_humidityNode->setProperty("unit").send("%");

    // Hardware part
    dhtu21 = new DHT_Unified(dht21Setting.get(), DHT21); // Initialize DHT sensor for normal 16mhz Arduino
    pinMode(dht21Setting.get(), OUTPUT);
    dhtu21->begin();
    Homie.getLogger() << "DHT21 on pin " << dht21Setting.get() << endl;
  }
  if(dht22Setting.wasProvided())
  {
    // Nodes part
    dht22_temperatureNode->setProperty("unit").send("c");
    dht22_humidityNode->setProperty("unit").send("%");

    // Hardware part
    dhtu22 = new DHT_Unified(dht22Setting.get(), DHT22); // Initialize DHT sensor for normal 16mhz Arduino
    pinMode(dht22Setting.get(), OUTPUT);
    dhtu22->begin();
    sensors.push_back(SensorNodePair(new DHT_Temperature(dhtu22), dht22_temperatureNode));
    sensors.push_back(SensorNodePair(new DHT_Humidity(dhtu22), dht22_humidityNode));
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
  Homie.loadSettings();

  sensors.setStorage(storage_array);

  if(dht11Setting.wasProvided())
  {
    dht11_temperatureNode = new HomieNode("dht11_temperature", "temperature");
    dht11_humidityNode = new HomieNode("dht11_humidity", "humidity");
    dht11_temperatureNode->advertise("unit");
    dht11_temperatureNode->advertise("degrees");
    dht11_humidityNode->advertise("unit");
    dht11_humidityNode->advertise("relative");
  }
  if(dht21Setting.wasProvided())
  {
    dht21_temperatureNode = new HomieNode("dht21_temperature", "temperature");
    dht21_humidityNode = new HomieNode("dht21_humidity", "humidity");
    dht21_temperatureNode->advertise("unit");
    dht21_temperatureNode->advertise("degrees");
    dht21_humidityNode->advertise("unit");
    dht21_humidityNode->advertise("relative");
  }
  if(dht22Setting.wasProvided())
  {
    dht22_temperatureNode = new SensorNode("dht22_temperature", TYPE_SENSOR);
    dht22_humidityNode = new SensorNode("dht22_humidity", TYPE_SENSOR);
    dht22_temperatureNode->advertise("unit");
    dht22_temperatureNode->advertise("degrees");
    dht22_humidityNode->advertise("unit");
    dht22_humidityNode->advertise("relative");
  }

  Homie.setup();
}


void loop() {
  Homie.loop();
}