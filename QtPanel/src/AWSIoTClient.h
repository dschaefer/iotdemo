/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
#ifndef SRC_AWSIOTCLIENT_H_
#define SRC_AWSIOTCLIENT_H_

#include <MQTTAsync.h>
#include <QObject>

class AWSIoTClient : public QObject {
	Q_OBJECT

public:
	AWSIoTClient(QString address, QString id, QObject *parent = NULL);

	void connect();

	void subscribe(QString topic);
	void publish(QString topic, QByteArray msg);

signals:
	void connected();
	void message(QString topic, QByteArray msg);

private:
	MQTTAsync client;
};

#endif /* SRC_AWSIOTCLIENT_H_ */
