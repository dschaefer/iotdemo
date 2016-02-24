/*
 * Receiver.cpp
 *
 *  Created on: Feb 22, 2016
 *      Author: dschaefer
 */

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
