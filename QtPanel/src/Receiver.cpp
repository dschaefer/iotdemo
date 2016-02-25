/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
#include "Receiver.h"
#include <QUdpSocket>

Receiver::Receiver(QObject * parent) : QObject(parent) {
	socket = new QUdpSocket(this);
	if (!socket->bind(QHostAddress("192.168.42.3"), 8080)) {
		fprintf(stderr, "bind failed\n");
	}

	connect(socket, SIGNAL(readyRead()), this, SLOT(processMessages()));
}

void Receiver::processMessages() {
	while (socket->hasPendingDatagrams()) {
		QByteArray datagram;
		datagram.resize(socket->pendingDatagramSize());
		socket->readDatagram(datagram.data(), datagram.size());
		emit message(QString(datagram));
	}
}

Receiver::~Receiver() {
	delete socket;
}
