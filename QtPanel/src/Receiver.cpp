/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
#include "Receiver.h"

Receiver::Receiver(QHostAddress address, quint16 port, QObject * parent)
: QObject(parent), socket(this)
{
	if (!socket.bind(address, port)) {
		fprintf(stderr, "bind failed\n");
	}

	connect(&socket, SIGNAL(readyRead()), this, SLOT(processMessages()));
}

void Receiver::processMessages() {
	while (socket.hasPendingDatagrams()) {
		QByteArray datagram;
		datagram.resize(socket.pendingDatagramSize());
		QHostAddress address;
		socket.readDatagram(datagram.data(), datagram.size(), &address);
		emit message(address, datagram);
	}
}

void Receiver::sendMessage(QHostAddress host, quint16 port, QByteArray msg) {
	QUdpSocket senderSocket(this);
	senderSocket.writeDatagram(msg, host, port);
	senderSocket.close();
}
