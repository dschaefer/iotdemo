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

#define init17 "/factory/sensor17/init"
#define status17 "/factory/sensor17/status"

Manager::Manager(QObject * parent)
: QObject(parent), iotClient("ssl://A2KECYFFLC558H.iot.us-east-1.amazonaws.com:8883", "BeagleBone", this), sensorReceiver(QHostAddress("192.168.42.2"), 8080, this)
{
	iotClient.connect();

	connect(&sensorReceiver, SIGNAL(message(QHostAddress, QByteArray)), this, SLOT(onMessage(QHostAddress, QByteArray)));
}

void Manager::onMessage(QHostAddress from, QByteArray msg) {
	if (from == sensor17) {
		qDebug("Received %s", QString(msg).toUtf8().data());
		QJsonParseError error;
		QJsonDocument jsonDoc = QJsonDocument::fromJson(msg, &error);
		QJsonObject json = jsonDoc.object();
		int state = json["state"].toInt(-1);
		if (state >= 0) {
			emit stateChanged(17, state);
			int time = json["time"].toInt(-1);
			QJsonObject status;
			status["state"] = state;
			if (time > 0) {
				status["time"] = time;
			}
			iotClient.publish(status17, QJsonDocument(status).toJson());
		}

		if (json["init"].toBool(false)) {
			QJsonObject init;
			init["init"] = true;
			iotClient.publish(init17, QJsonDocument(init).toJson());
		}
	}
}

void Manager::setState(int sensor, int state) {
	if (sensor == 17) {
		QJsonObject json;
		json["state"] = state;
		sensorReceiver.sendMessage(sensor17, 8081, QJsonDocument(json).toJson());
	}
}
