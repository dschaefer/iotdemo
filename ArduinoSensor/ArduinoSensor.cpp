#include <Arduino.h>
#include <Adafruit_CC3000.h>

#define WLAN_SSID	"dasWifi"
#define WLAN_PASS	"eclipseide"
#define WLAN_SECURITY WLAN_SEC_WPA2

#define PIN_CC3000_IRQ	3
#define PIN_CC3000_VBAT	5
#define PIN_CC3000_CS	10

#define PIN_NEOPIXEL		6
#define PIN_SENSOR		7

Adafruit_CC3000 cc3000 = Adafruit_CC3000(PIN_CC3000_CS, PIN_CC3000_IRQ, PIN_CC3000_VBAT);

void setup() {
	Serial.begin(115200);

	Serial.print(F("Initializing the CC3000 ... "));
	if (!cc3000.begin()) {
		Serial.println(F("failed"));
		while (1);
	}
	Serial.println(F("passed"));

	uint8_t macAddress[6] = { 0x08, 0x00, 0x28, 0x01, 0x79, 0xBB };
	if (!cc3000.setMacAddress(macAddress)) {
		Serial.println(F("Failed trying to update the MAC address"));
		while(1);
	}

	uint8_t major, minor;
	if (!cc3000.getFirmwareVersion(&major, &minor)) {
		Serial.println(F("CC3000 get firmware version failed"));
		while (1);
	}
	Serial.print(F("CC3000 firmware version "));
	Serial.print(major);
	Serial.print(F("."));
	Serial.println(minor);

	Serial.print(F("Deleting profiles ... "));
	if (!cc3000.deleteProfiles()) {
		Serial.println(F("failed"));
		while (1);
	}
	Serial.println(F("passed"));

	Serial.print(F("Connecting to "));
	Serial.print(F(WLAN_SSID));
	Serial.print(F(" ... "));

	uint32_t ipAddress = cc3000.IP2U32(192, 168, 42, 2);
	uint32_t netMask = cc3000.IP2U32(255, 255, 255, 0);
	uint32_t defaultGateway = cc3000.IP2U32(192, 168, 42, 1);
	uint32_t dns = cc3000.IP2U32(8, 8, 4, 4);
	if (!cc3000.setStaticIPAddress(ipAddress, netMask, defaultGateway, dns)) {
		Serial.println(F("failed to set address"));
		while (1);
	}

	if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
		Serial.println(F("failed to connect"));
		while (1);
	}

	Serial.print(F("almost there ... "));
	while (cc3000.getStatus() != STATUS_CONNECTED) {
		delay(200);
	}
	Serial.println(F("connected"));

	Serial.println(F("Initialization complete."));
}

int n = 0;

void loop() {
	char msg[64];
	sprintf(msg, "Arduino says hi %d", n++);

	uint32_t beagleboneIP = cc3000.IP2U32(192, 168, 42, 3);
	Adafruit_CC3000_Client client = cc3000.connectUDP(beagleboneIP, 8080);
	if (!client.connected()) {
		Serial.println(F("Connecting to beaglebone failed"));
		while (1);
	}

	Serial.println(msg);
	client.write(msg, strlen(msg) + 1);
	client.close();

	delay(5000);
}
