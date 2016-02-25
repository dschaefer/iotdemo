/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <QObject>

class QUdpSocket;

class Receiver : public QObject {
	Q_OBJECT
public:
	Receiver(QObject * parent = NULL);
	virtual ~Receiver();

signals:
	void message(QByteArray msg);

private slots:
	void processMessages();

private:
	QUdpSocket * socket;
};

#endif /* RECEIVER_H_ */
