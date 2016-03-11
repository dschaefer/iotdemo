/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
#include <src/Manager.h>
#include <QJsonDocument>
#include <QJsonObject>

static QHostAddress sensor17("192.168.42.17");
#define SENSOR_PORT 8081

//#define MQTT_URL "ssl://127.0.0.1:8883"
#define MQTT_URL "ssl://A2KECYFFLC558H.iot.us-east-1.amazonaws.com:8883"

Manager::Manager(QObject * parent) :
		QObject(parent), connected(false), iotClient(MQTT_URL, "BeagleBone", this), sensorReceiver(
				QHostAddress("192.168.42.2"), 8080, this) {
	connect(&sensorReceiver, SIGNAL(message(QHostAddress, QByteArray)), this,
			SLOT(onSensorMessage(QHostAddress, QByteArray)));
	connect(&iotClient, SIGNAL(connected()), this, SLOT(onServerConnected()));
	connect(&iotClient, SIGNAL(message(QString, QByteArray)), this,
			SLOT(onServerMessage(QString, QByteArray)));

	iotClient.connect();
}

void Manager::onServerConnected() {
	connected = true;
	iotClient.subscribe("/factory/in");
}

void Manager::onSensorMessage(QHostAddress from, QByteArray msg) {
	if (from == sensor17) {
		QJsonParseError error;
		QJsonObject report = QJsonDocument::fromJson(msg, &error).object();

		QJsonObject upreport;
		upreport["sensor"] = "sensor17";

		int state = report["state"].toInt(-1);
		if (state >= 0) {
			emit stateChanged(17, state);
			upreport["state"] = state;
		}

		int time = report["time"].toInt(-1);
		if (time >= 0) {
			upreport["time"] = time;
		}

		if (report["init"].toBool(false)) {
			upreport["init"] = true;
		}

		if (connected) {
			iotClient.publish("/factory/out", QJsonDocument(upreport).toJson());
		}
	}
}

void Manager::onServerMessage(QString topic, QByteArray msg) {
	QJsonParseError error;
	QJsonObject message = QJsonDocument::fromJson(msg, &error).object();

	if (message["sensor"] == "sensor17") {
		QJsonObject sensor;

		int thresh = message["thresh"].toInt(-1);
		if (thresh >= 0) {
			sensor["thresh"] = thresh;
		}

		int state = message["state"].toInt(-1);
		if (state >= 0) {
			sensor["state"] = state;
		}

		sensorReceiver.sendMessage(sensor17, SENSOR_PORT, QJsonDocument(sensor).toJson());
	}
}

void Manager::setState(int sensor, int state) {
	if (sensor == 17) {
		QJsonObject sensor;
		sensor["state"] = state;
		sensorReceiver.sendMessage(sensor17, SENSOR_PORT, QJsonDocument(sensor).toJson());
	}
}
