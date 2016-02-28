/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
#include <Arduino.h>
#include <Adafruit_CC3000.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <utility/socket.h>

#define WLAN_SSID	"dasWifi"
#define WLAN_PASS	"eclipseide"
#define WLAN_SECURITY WLAN_SEC_WPA2

#define PIN_CC3000_IRQ	3
#define PIN_CC3000_VBAT	5
#define PIN_CC3000_CS	10

#define PIN_NEOPIXEL		6
#define PIN_SENSOR		7

#define NUM_NEOPIXEL		4

#define SEND_PORT 8080
#define RECV_PORT 8081

Adafruit_CC3000 cc3000 = Adafruit_CC3000(PIN_CC3000_CS, PIN_CC3000_IRQ,
		PIN_CC3000_VBAT);
Adafruit_CC3000_Client server;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL,
		NEO_GRB + NEO_KHZ800);

static void setPixels(uint8_t r, uint8_t g, uint8_t b,
		uint8_t brightness = 0xff) {
	pixels.begin();
	for (int i = 0; i < NUM_NEOPIXEL; ++i) {
		pixels.setPixelColor(i, r, g, b);
	}
	pixels.setBrightness(brightness);
	pixels.show();
}

static void failMode() {
	while (true) {
		setPixels(0xff, 0, 0);
		delay(1000);
		setPixels(0, 0, 0);
	}
}

Adafruit_CC3000_Client initServer() {
	int32_t s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s < 0) {
		Serial.println(F("Failed to allocate server socket"));
		failMode();
	}

	sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(RECV_PORT);
	address.sin_addr.s_addr = 0;

	if (bind(s, (sockaddr*) &address, sizeof(address)) < 0) {
		Serial.println(F("Failed to bind server socket"));
		failMode();
	}

	return Adafruit_CC3000_Client(s);
}

void setup() {
	Serial.begin(115200);

	setPixels(0, 0, 0xff, 0x80);

	Serial.print(F("Initializing the CC3000 ... "));
	if (!cc3000.begin()) {
		Serial.println(F("failed"));
		while (1)
			;
	}
	Serial.println(F("passed"));

	uint8_t macAddress[6] = { 0x08, 0x00, 0x28, 0x01, 0x79, 0xBB };
	if (!cc3000.setMacAddress(macAddress)) {
		Serial.println(F("Failed trying to update the MAC address"));
		failMode();
	}

	uint8_t major, minor;
	if (!cc3000.getFirmwareVersion(&major, &minor)) {
		Serial.println(F("CC3000 get firmware version failed"));
		failMode();
	}
	Serial.print(F("CC3000 firmware version "));
	Serial.print(major);
	Serial.print(F("."));
	Serial.println(minor);

	Serial.print(F("Deleting profiles ... "));
	if (!cc3000.deleteProfiles()) {
		Serial.println(F("failed"));
		failMode();
	}
	Serial.println(F("passed"));

	Serial.print(F("Connecting to "));
	Serial.print(F(WLAN_SSID));
	Serial.print(F(" ... "));

	uint32_t ipAddress = cc3000.IP2U32(192, 168, 42, 17);
	uint32_t netMask = cc3000.IP2U32(255, 255, 255, 0);
	uint32_t defaultGateway = cc3000.IP2U32(192, 168, 42, 1);
	uint32_t dns = cc3000.IP2U32(8, 8, 4, 4);
	if (!cc3000.setStaticIPAddress(ipAddress, netMask, defaultGateway, dns)) {
		Serial.println(F("failed to set address"));
		failMode();
	}

	if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
		Serial.println(F("failed to connect"));
		failMode();
	}

	Serial.print(F("almost there ... "));
	while (cc3000.getStatus() != STATUS_CONNECTED) {
		delay(200);
	}
	Serial.println(F("connected"));

	server = initServer();

	Serial.println(F("Initialization complete."));

	pinMode(PIN_SENSOR, INPUT);
}

void sendMessage(JsonObject & msg) {
	char buffer[64];
	size_t n = msg.printTo(buffer, sizeof(buffer));
	buffer[n] = 0;

	Adafruit_CC3000_Client client = cc3000.connectUDP(cc3000.IP2U32(192, 168, 42, 2), SEND_PORT);
	client.write(buffer, n);
	client.close();

	Serial.print("Sent ");
	Serial.println(buffer);
}

static int state = -1;
static long lastTime = 0;
static int needInit = 1;

void setState(int newState) {
	if (newState) {
		setPixels(0xff, 0xff, 0, 0x40);
	} else {
		setPixels(0, 0xff, 0, 0x40);
	}

	StaticJsonBuffer<64> jsonBuffer;
	JsonObject &json = jsonBuffer.createObject();
	json["state"] = newState;

	if (needInit) {
		json["init"] = true;
	}

	if (newState == 1) {
		long thisTime = millis();
		if (lastTime > 0) {
			json["time"] = thisTime - lastTime;
		}
		lastTime = thisTime;
	}

	sendMessage(json);

	state = newState;
}

static int sequence = 0;
static int override = 0;
static long onTime;

void loop() {
	if (++sequence > 25) {
		server.close();
		server = initServer();
		if (false && needInit) {
			StaticJsonBuffer<32> jsonBuffer;
			JsonObject &json = jsonBuffer.createObject();
			json["init"] = true;
			sendMessage(json);
		}
		sequence = 0;
	} else if (server.available()) {
		char buffer[32];
		int n = server.read(buffer, sizeof(buffer));
		buffer[n] = 0;
		StaticJsonBuffer<16> jsonBuffer;
		JsonObject &json = jsonBuffer.parseObject(buffer);
		int newState = json["state"];
		if (newState == 1) {
			setState(newState);
			override = 1;
			onTime = millis();
		}
	} else if (override && millis() - onTime > 4000) {
		setState(0);
		override = 0;
	} else if (!override) {
		int newState = digitalRead(PIN_SENSOR);
		if (newState != state) {
			setState(newState);
		}
	}

	delay(200);
}
