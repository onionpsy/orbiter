/*
 * 2014 - Patrice.S
 */

#include <GSM.h>
#include <TinyGPS++.h>

#include <AltSoftSerial.h>
#include "floatToString.h"


/* PINS Configuration (GSM Board)
 * (1  : gsm tx)
 * (2  : gsm rx)
 *  4  : humidity DHT data
 *  9  : gps tx
 *  8  : gps rx
 *  10 : sd card (used to select card)
 *  11 : sd card
 *  12 : sd card
 *  13 : sd card (?)
 *  a5 : pressure SCL
 *  a4 : pressure SDA
 *
 *
 */


/*
 * Main Configuration
 */

// General conf
#define DELAY 6000

// GSM settings
#define GSMPINNUMBER "" // PIN CODE !
#define GPRS_APN "internet" 
#define GPRS_LOGIN ""
#define GPRS_PASSWORD ""

// GPRS connection
char server[] = "quarkdev.com";
char path[] = "/~patrice/orbiter/index.php";
int port = 80;

// GPS config
#define GPS_RX_PIN 0
#define GPS_RATE 9600 // gps baud rate (datasheet)


// Init lib ref
GSM gsmAccess(true);
GPRS gprsAccess;
GSMClient client;
TinyGPSPlus gps;


AltSoftSerial ss;


void setup() {
	Serial.begin(4800); // or 4800
	Serial.println(F("INIT..."));

	ss.begin(GPS_RATE);

	boolean gsmConnected = false;

	// Test GSM connection
	while (!gsmConnected) {
		if ((gsmAccess.begin(GSMPINNUMBER) == GSM_READY) &
			(gprsAccess.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
			gsmConnected = true;
		} else {
			delay(1000);
		}
	}

	// Test GPRS connect (send HTTP request)
	if (client.connect(server, port)) {
		client.print("GET ");
		client.print(path);
		client.println(" HTTP/1.0");
		client.println();
		Serial.println("OK");
	} else {
		Serial.println("NOT OK");
	}
}

void loop() {
	String gpsData = "";

	// Read GPS data
	while (ss.available() > 0) { 
		gps.encode(ss.read());
	}
	
	// Parse values (for SMS)
	char buffer[25];
	String lat = floatToString(buffer, gps.location.lat(), 5);
	String lng = floatToString(buffer, gps.location.lng(), 5);
	String alt = String(gps.altitude.meters());

	// POST data request
	gpsData += "lat=" + lat;
	gpsData += "&lng=" + lng;
	gpsData += "&alt=" + alt;
	gpsData += "&hour=" + (String) gps.time.hour();
	gpsData += "&minute=" + (String) gps.time.minute();
	gpsData += "&second=" + (String) gps.time.second();
	gpsData += "&speed=" + (String) gps.speed.kmph();
	gpsData += "&sat=" + (String) gps.satellites.value();

	Serial.println(F("GPS DATA : "));
	Serial.println(gpsData);

	if (client.connect(server, port)) {
		client.print("POST ");
		client.print(path);
		client.print(" HTTP/1.1");
		client.print("Host: ");
		client.println(server);
		client.println("Content-type: application/x-www-form-urlencoded");
		client.print("Content-Length: ");
		client.println(gpsData.length());
		client.println();
		client.print(gpsData);
	}

	// End HTTP connection
	if (client.connected()) {
		client.stop();
	}

	delay(DELAY);
}

