/*
 * Receiver.h
 *
 *  Created on: Feb 22, 2016
 *      Author: dschaefer
 */

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
	void message(QString msg);

private slots:
	void processMessages();

private:
	QUdpSocket * socket;
};

#endif /* RECEIVER_H_ */
