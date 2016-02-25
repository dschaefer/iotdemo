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

Manager::Manager(QObject * parent)
: QObject(parent), iotClient("ssl://192.168.42.1:8883", "BeagleBone", this), sensorReceiver(QHostAddress("192.168.42.2"), 8080, this)
{
	//iotClient.connect();

	connect(&sensorReceiver, SIGNAL(message(QHostAddress, QByteArray)), this, SLOT(onMessage(QHostAddress, QByteArray)));
}

void Manager::onMessage(QHostAddress from, QByteArray msg) {
	if (from == sensor17) {
		QJsonParseError error;
		QJsonDocument jsonDoc = QJsonDocument::fromJson(msg, &error);
		QJsonObject json = jsonDoc.object();
		int state = json["state"].toInt(-1);
		emit stateChanged(17, state);
	}
}

void Manager::setState(int sensor, int state) {
	if (sensor == 17) {
		QJsonObject json;
		json["state"] = state;
		QJsonDocument jsonDoc = QJsonDocument(json);
		sensorReceiver.sendMessage(sensor17, 8081, jsonDoc.toJson());
	}
}
