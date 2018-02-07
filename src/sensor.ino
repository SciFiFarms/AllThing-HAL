#include <Homie.h>
//#include <GDBStub.h>

float temperature;
float humidity;
const int TEMP_INTERVAL = 10;                   // seconds
unsigned long last_temp_sent = 0;

HomieNode temperatureNode("temperature", "temperature");
HomieNode humidityNode("humidity", "humidity");

void loopHandler() {
  if (millis() - last_temp_sent >= TEMP_INTERVAL * 1000UL || last_temp_sent == 0) 
  {
       
//                sht30.get();
//                temperature = sht30.cTemp;
//                humidity = sht30.humidity;

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    if (temperatureNode.setProperty("degrees").send(String(temperature)))
    {
      last_temp_sent = millis();
    }
 //1j   Homie.setNodeProperty(humidityNode, "percent", String(humidity), true);
    humidityNode.setProperty("percent").send(String(humidity));
//  delay(1000);
  }
}

void setupHandler() {
  temperatureNode.setProperty("unit").send("C");
  humidityNode.setProperty("unit").send("%");
}



void setup() {
  Serial.begin(115200);
//#ifdef ENABLE_GDB
	//gdbstub_init();
//#endif
  Homie.setSetupFunction(setupHandler);
  Homie.setLoopFunction(loopHandler);
  Homie_setFirmware("boxbot", "1.0.0");
  Homie.setup();
}

void loop() {
  Homie.loop();
}
