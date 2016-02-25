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

#define WLAN_SSID	"dasWifi"
#define WLAN_PASS	"eclipseide"
#define WLAN_SECURITY WLAN_SEC_WPA2

#define PIN_CC3000_IRQ	3
#define PIN_CC3000_VBAT	5
#define PIN_CC3000_CS	10

#define PIN_NEOPIXEL		6
#define PIN_SENSOR		7

#define NUM_NEOPIXEL		4

#define UDP_PORT 8080

Adafruit_CC3000 cc3000 = Adafruit_CC3000(PIN_CC3000_CS, PIN_CC3000_IRQ, PIN_CC3000_VBAT);
Adafruit_CC3000_Client client;
uint32_t beagleboneIP;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

static void setPixels(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness = 0xff) {
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

void setup() {
	Serial.begin(115200);

	setPixels(0, 0, 0xff, 0x80);

	Serial.print(F("Initializing the CC3000 ... "));
	if (!cc3000.begin()) {
		Serial.println(F("failed"));
		while (1);
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

	uint32_t ipAddress = cc3000.IP2U32(192, 168, 42, 2);
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

	beagleboneIP = cc3000.IP2U32(192, 168, 42, 3);
	client = cc3000.connectUDP(beagleboneIP, UDP_PORT);
	if (!client.connected()) {
		Serial.println(F("Connecting to BeagleBone failed"));
		failMode();
	}

	Serial.println(F("Initialization complete."));

	pinMode(PIN_SENSOR, INPUT);
}

static int state = -1;

void loop() {
	int newState = digitalRead(PIN_SENSOR);
	if (newState != state) {
		if (newState) {
			setPixels(0xff, 0xff, 0, 0x40);
		} else {
			setPixels(0, 0xff, 0, 0x40);
		}
		state = newState;

		StaticJsonBuffer<16> jsonBuffer;
		JsonObject &json = jsonBuffer.createObject();
		json["state"] = state;

		char buffer[16];
		size_t n = json.printTo(buffer, sizeof(buffer));

		if (!client.connected()) {
			Serial.println(F("Reconnecting"));
			client.connect(beagleboneIP, UDP_PORT);
		}

		Serial.print(F("Sending "));
		client.write(buffer, n + 1);
		client.flush();
		Serial.println(buffer);
	}

	delay(200);
}
