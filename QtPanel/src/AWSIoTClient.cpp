/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
#include "AWSIoTClient.h"
#include <QDebug>
#include <errno.h>

extern "C" {
#include "Log.h"
}

#define ADDRESS "ssl://192.168.42.1:8883"

static void onConnect(void* context, MQTTAsync_successData* response) {
	emit ((AWSIoTClient *) context)->connected();
}

static void onConnectFailure(void* context, MQTTAsync_failureData* response) {
	qFatal("Connect failed %s", response->message);
}

static int messageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message) {
    emit ((AWSIoTClient *) context)->message(topicName, QByteArray((char *) message->payload, message->payloadlen));

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}

void AWSIoTClient::connect() {
	MQTTAsync_connectOptions connectOptions = MQTTAsync_connectOptions_initializer;
	connectOptions.keepAliveInterval = 20;
	connectOptions.cleansession = 1;
	connectOptions.onSuccess = onConnect;
	connectOptions.onFailure = onConnectFailure;
	connectOptions.context = this;
	MQTTAsync_SSLOptions sslOptions = MQTTAsync_SSLOptions_initializer;
	sslOptions.trustStore = "/Users/dschaefer/cloud/iot/root-certificate.pem";
	sslOptions.keyStore = "/Users/dschaefer/cloud/iot/6f8159c6b8-certificate.pem.crt";
	sslOptions.privateKey = "/Users/dschaefer/cloud/iot/6f8159c6b8-private.pem.key";
	connectOptions.ssl = &sslOptions;

	int rc = MQTTAsync_connect(client, &connectOptions);
	if (rc != MQTTASYNC_SUCCESS) {
		qFatal("Failed to start connect, return code %d\n", rc);
	}
}

static void connectionLost(void *context, char *cause)
{
	qWarning("Connection lost %s, reconnecting", cause);
	((AWSIoTClient *) context)->connect();
}

AWSIoTClient::AWSIoTClient(QString address, QString id, QObject *parent)
	: QObject(parent)
{
	int rc = MQTTAsync_create(&client, address.toUtf8(), id.toUtf8(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
	if (rc != MQTTASYNC_SUCCESS) {
		qFatal("Failed to create client %d\n", rc);
	}

	rc = MQTTAsync_setCallbacks(client, this, connectionLost, messageArrived, NULL);
	if (rc != MQTTASYNC_SUCCESS) {
		qFatal("Failed to set callbacks %d\n", rc);
	}
}

void onSubscribed(void* context, MQTTAsync_successData* response) {

}

void onSubscribeFailure(void* context, MQTTAsync_failureData* response) {
	qFatal("Subscribe failed %s", response->message);
}

void AWSIoTClient::subscribe(QString topic) {
	MQTTAsync_responseOptions options = MQTTAsync_responseOptions_initializer;
	options.onSuccess = onSubscribed;
	options.onFailure = onSubscribeFailure;
	options.context = this;

	int rc = MQTTAsync_subscribe(client, topic.toUtf8(), 0, &options);
	if (rc != MQTTASYNC_SUCCESS) {
		qFatal("Failed so subscribe to topic %d", rc);
	}
}

void onSent(void* context, MQTTAsync_successData* response) {

}

void onSendFailure(void* context, MQTTAsync_failureData* response) {
	qFatal("Send failed %s", response->message);
}

void AWSIoTClient::publish(QString topic, QByteArray msg) {
	MQTTAsync_responseOptions options = MQTTAsync_responseOptions_initializer;
	options.onSuccess = onSent;
	options.onFailure = onSendFailure;
	options.context = this;

	MQTTAsync_message message = MQTTAsync_message_initializer;
	message.payload = msg.data();
	message.payloadlen = msg.size();
	message.qos = 0;
	message.retained = 0;

	int rc = MQTTAsync_sendMessage(client, topic.toUtf8(), &message, &options);
	if (rc != MQTTASYNC_SUCCESS) {
		qFatal("Failed to start sendMessage, return code %d\n", rc);
	}
}
