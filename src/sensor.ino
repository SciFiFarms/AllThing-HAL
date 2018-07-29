#include <Homie.h>
//#include <GDBStub.h>
#include <Adafruit_Sensor.h>

// DHT sensor consts/vars
  #include "DHT.h"
  #define DHTPIN     D2     // What pin we're connected to
  #define DHTTYPE    DHT22  // Can be DHT11, DHT22 (AM2302), DHT21 (AM2301)
  DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino
  
  //DHT dht;

const int MEASURE_INTERVAL = 1; // How often to poll DHT22 for temperature and humidity

unsigned long lastMeasureSent = 0;

HomieNode temperatureNode("temperature", "temperature");
HomieNode humidityNode("humidity", "humidity");

void loopHandler() {
  if (millis() - lastMeasureSent >= MEASURE_INTERVAL * 1000UL || lastMeasureSent == 0) {
    float temperature = dht.readTemperature(); // Read temperature as Celsius
    float humidity = dht.readHumidity(); // Read humidity as relative [0-100]%
    if (isnan(temperature) || isnan(humidity)) {
      Homie.getLogger() << F("Failed to read from DHT sensor!");
    } else {
      Homie.getLogger() << F("Temperature: ") << temperature << " °C" << endl;
      Homie.getLogger() << F("Humidity   : ") << humidity << " %" << endl;
      temperatureNode.setProperty("degrees").send(String(temperature));
      humidityNode.setProperty("relative").send(String(humidity));
    }
    lastMeasureSent = millis();
  }
}

void setupHandler() {
  // Nodes part
  temperatureNode.setProperty("unit").send("c");
  humidityNode.setProperty("unit").send("%");

  // Hardware part
  pinMode(DHTPIN, OUTPUT);
  dht.begin();
  Homie.getLogger() << "DHT " << DHTTYPE << " on pin " << DHTPIN << endl;
}

void setup() {
  Serial.begin(115200); // Required to enable serial output
  Homie.setLedPin(15, HIGH);
  //Homie.enableBuiltInLedIndicator(false);
  //dht.setup(2); // data pin 2
  Serial << "HELLO" << endl;

  Homie_setFirmware("boxbot", "1.0.0");
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  temperatureNode.advertise("unit");
  temperatureNode.advertise("degrees");

  humidityNode.advertise("unit");
  humidityNode.advertise("relative");

  Homie.setup();
}


void loop() {
  Homie.loop();
}