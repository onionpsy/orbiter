#include <TinyGPS.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <GSM.h>

#define DHTPIN 4 
#define DHTTYPE DHT11


// ---------------------------------- //
//
// Main configuration
//

// GSM settings
#define GSMPINNUMBER "" // PIN CODE !
#define GPRS_APN "internet" 
#define GPRS_LOGIN ""
#define GPRS_PASSWORD ""

// GPRS connection
char server[] = "quarkdev.com";
char path[] = "/~patrice/orbiter/index.php";
int port = 80;

// ---------------------------------- //

// Init lib ref
DHT dht(DHTPIN, DHTTYPE); // Humidity Sensor
Adafruit_BMP085 bmp; // Pressure Sensor
GSM gsmAccess(true);
GPRS gprsAccess;
GSMClient client;
GSMScanner scannerNetworks;
//GSMModem modem;

// GSM default values
String IMEI = "";
String errortext = "ERROR";

void setup() {
	Serial.println("START");
	Serial.begin(9600);
	dht.begin();
	bmp.begin();
	boolean gsmConnected = false;

	// Enable GSM connection
	while (!gsmConnected) {
		if ((gsmAccess.begin(GSMPINNUMBER) == GSM_READY) &
			(gprsAccess.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
			gsmConnected = true;
			Serial.println("GSM module connected");
		} else {
			Serial.println("GSM module not connected");
			delay(1000);
		}
	}

	// Init GPRS connection
	if (client.connect(server, port)) {
		Serial.print("Send HTTP resquest to : ");

		// HTTP request
		client.print("GET ");
		client.print(path);
		client.println(" HTTP/1.0");
		client.println();
		// TODO : add data with POST
	} else {
		Serial.println("Connection failed");
	}
}

void loop() {
	// GSM
	//Serial.print("Networks : ");
	//Serial.println(scannerNetworks.readNetworks()); // return list
	//Serial.print("Current Carrier : ");
	//Serial.println(scannerNetworks.getCurrentCarrier());
	//Serial.print("Serial strength : ");
	//Serial.println(scannerNetworks.getSignalStrength());

	// GPRS connection
	Serial.println("--- HTTP Connection ---");
	if (!client.available() && !client.connected()) {
		Serial.println("Disconnecting from client");
		client.stop();
	} else {
		
	}

	if (client.available()) {
		char c = client.read();
		Serial.println("Message from server : ");
		Serial.print(c);
	}
	           
	// Humidity Sensor
	float h = dht.readHumidity();
	float t = dht.readTemperature();
	Serial.println("--- Humidity Sensor ---");
	Serial.print("Humidity is ");
	Serial.print(h,1);
	Serial.print(" / Temperature is ");
	Serial.println(t,1);

	// Pressure Sensor
	Serial.println("--- Pressure Sensor ---");
	Serial.print("Temperature = ");
	Serial.print(bmp.readTemperature());
	Serial.print(" *C");

	Serial.print(" / Pressure = ");
	Serial.print(bmp.readPressure());
	Serial.println(" Pa");

	// Calculate altitude assuming 'standard' barometric
	// pressure of 1013.25 millibar = 101325 Pascal
	Serial.print("Altitude = ");
	Serial.print(bmp.readAltitude());
	Serial.print(" meters");
	Serial.print(" / Pressure at sealevel (calculated) = ");
	Serial.print(bmp.readSealevelPressure());
	Serial.println(" Pa");
	// you can get a more precise measurement of altitude
	// if you know the current sea level pressure which will
	// vary with weather and such. If it is 1015 millibars
	// that is equal to 101500 Pascals.
	Serial.print("Real altitude = ");
	Serial.print(bmp.readAltitude(101500));
	Serial.println(" meters");
	Serial.println("");
	delay(2000);
}


