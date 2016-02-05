/*
 * AWSIoTClient.h
 *
 *  Created on: Feb 3, 2016
 *      Author: dschaefer
 */

#ifndef SRC_AWSIOTCLIENT_H_
#define SRC_AWSIOTCLIENT_H_

#include <MQTTClient.h>
#include <QObject>

class AWSIoTClient : public QObject {
	Q_OBJECT

public:
	AWSIoTClient(QObject *parent = NULL);

	Q_INVOKABLE void subscribe(QString topic);
	Q_INVOKABLE void sendMessage(QString topic, QString msg);

signals:
	void message(QString topic, QString msg);

private:
	MQTTClient client;
};

#endif /* SRC_AWSIOTCLIENT_H_ */
