/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
#ifndef MANAGER_H_
#define MANAGER_H_

#include <QObject>
#include "AWSIoTClient.h"
#include "Receiver.h"

class Manager : public QObject {
	Q_OBJECT

public:
	Manager(QObject * parent = NULL);

	Q_INVOKABLE void setState(int sensor, int state);

signals:
	void stateChanged(int sensor, int state);

private slots:
	void onSensorMessage(QHostAddress from, QByteArray msg);
	void onServerMessage(QString topic, QByteArray msg);

private:
	AWSIoTClient iotClient;
	Receiver sensorReceiver;
};

#endif /* MANAGER_H_ */
