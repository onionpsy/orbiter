#include <TinyGPS.h>
#include <DHT.h>

// https://github.com/adafruit/DHT-sensor-library/blob/master/examples/DHTtester/DHTtester.ino
//int humiditySensor = 4; // digital
#define DHTPIN 4 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
	Serial.println("START");
	Serial.begin(9600);
	dht.begin();
}

void loop() {
	float h = dht.readHumidity();
	float t = dht.readTemperature();
	Serial.print("Humidity is ");
	Serial.print(h,1);
	Serial.println(" ");
	Serial.println("Temperature is ");
	Serial.println(t,1);
	delay(1000);
}


