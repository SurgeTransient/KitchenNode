/*
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Alex Arent
 *
 * DESCRIPTION
 * Sensor is used in my Kitchen. It was based
 * off the Motion sensor MySensors example
 */

// Enable debug prints
//#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_RF24
#define MY_SPLASH_SCREEN_DISABLED

#include <MySensors.h>
#include <DHT.h>

uint32_t SLEEP_TIME = 120000; // Sleep time between reports (in milliseconds)
                              // 21600000 = 6 hours
                              
#define DIGITAL_INPUT_SENSOR 3   // The digital input you attached your motion sensor.  (Only 2 and 3 generates interrupt!)
#define DHT_DATA_PIN 4

#define LIGHT_SENSOR_INPUT A0

#define LIGHT_SENSOR_SLOPE 0.099 //6 adc counts when dark, 1010 adc counts when lit, y =m(x) 
#define LIGHT_SENSOR_OFFSET 0.597

#define CHILD_ID_Motion 1   // Id of the sensor children
#define CHILD_ID_TEMP 2
#define CHILD_ID_HUM 3
#define CHILD_ID_LIGHT 4

// Initialize messages
MyMessage msgMotion(CHILD_ID_Motion, V_TRIPPED);
MyMessage msgTemp(CHILD_ID_TEMP,V_TEMP);
MyMessage msgHum(CHILD_ID_HUM,V_HUM);
MyMessage msgLight(CHILD_ID_LIGHT,V_LIGHT);

DHT dht;
void setup()
{
	pinMode(DIGITAL_INPUT_SENSOR, INPUT_PULLUP);   
  pinMode(LIGHT_SENSOR_INPUT,INPUT);
  
  dht.setup(DHT_DATA_PIN); // set data pin of DHT sensor
  wait(dht.getMinimumSamplingPeriod());
}

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("Kitchen Node", "Version 1.0");
  
	// Register all sensors to gw (they will be created as child devices)
	present(CHILD_ID_Motion, S_MOTION);
  present(CHILD_ID_TEMP,S_TEMP);
  present(CHILD_ID_HUM,S_HUM);
  present(CHILD_ID_LIGHT,S_LIGHT);
}

void loop()
{
	//Read state of sensors
	bool tripped = digitalRead(DIGITAL_INPUT_SENSOR) == HIGH; 
  float humidity = dht.getHumidity();
  float temperature = dht.toFahrenheit(dht.getTemperature());
  float lightLevel = analogRead(LIGHT_SENSOR_INPUT)*LIGHT_SENSOR_SLOPE - LIGHT_SENSOR_OFFSET; 

  #ifdef MY_DEBUG
  	Serial.println(tripped);
    Serial.println(temperature,1);
    Serial.println(humidity,1);
    Serial.println(lightLevel,1);
  #endif
  
	send(msgMotion.set(tripped?"1":"0"));  // Send tripped value to gw
  send(msgTemp.set(temperature,1));
  send(msgHum.set(humidity,1));
  send(msgLight.set(lightLevel,1));

	// Sleep until interrupt comes in on motion sensor. Send update every two minute.
	sleep(digitalPinToInterrupt(DIGITAL_INPUT_SENSOR), CHANGE, SLEEP_TIME);
}
