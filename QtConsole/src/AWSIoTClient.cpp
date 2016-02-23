/*
 * AWSIoTClient.cpp
 *
 *  Created on: Feb 3, 2016
 *      Author: dschaefer
 */

#include "AWSIoTClient.h"
#include <QDebug>
#include <errno.h>

extern "C" {
#include "Log.h"
}

static void connectionLost(void* context, char* cause) {
	fprintf(stderr, "Connection lost: %s\n", cause);
}

static int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *m) {
	AWSIoTClient * iotClient = (AWSIoTClient *) context;
	iotClient->message(QLatin1String(topicName), QLatin1String((char *) m->payload, m->payloadlen));
	return 1;
}

static void deliveryComplete(void* context, MQTTClient_deliveryToken dt) {

}

AWSIoTClient::AWSIoTClient(QObject *parent)
	: QObject(parent)
{
//    int rc = MQTTClient_create(&client, "ssl://A2KECYFFLC558H.iot.us-east-1.amazonaws.com:8883",
//    		"BeagleBone", MQTTCLIENT_PERSISTENCE_NONE, NULL);
	int rc = MQTTClient_create(&client, "ssl://localhost:8883", "BeagleBone", MQTTCLIENT_PERSISTENCE_NONE, NULL);
	if (rc != MQTTCLIENT_SUCCESS) {
		fprintf(stderr, "Failed to create client %d\n", rc);
	}

#if 0
	rc = MQTTClient_setCallbacks(client, this, connectionLost, messageArrived, deliveryComplete);
	if (rc != MQTTCLIENT_SUCCESS) {
		fprintf(stderr, "Failed to set callbacks %d\n", rc);
	}
#endif

	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
	conn_opts.keepAliveInterval = 10;
	conn_opts.reliable = 0;
	conn_opts.cleansession = 1;
	conn_opts.ssl = &ssl_opts;
	ssl_opts.trustStore = "/Users/dschaefer/cloud/iot/root-certificate.pem";
	ssl_opts.keyStore = "/Users/dschaefer/cloud/iot/6f8159c6b8-certificate.pem.crt";
	ssl_opts.privateKey = "/Users/dschaefer/cloud/iot/6f8159c6b8-private.pem.key";

	fprintf(stderr, "Connecting\n");
	rc = MQTTClient_connect(client, &conn_opts);
	if (rc != MQTTCLIENT_SUCCESS) {
		fprintf(stderr, "Failed to connect %d\n", errno);
		exit(1);
	} else {
		fprintf(stderr, "Connected\n");
	}
}

void AWSIoTClient::subscribe(QString topic) {
	int rc = MQTTClient_subscribe(client, topic.toLatin1().data(), 0);
	if (rc != MQTTCLIENT_SUCCESS) {
		fprintf(stderr, "Failed to subscribe %d\n", rc);
	}
}

void AWSIoTClient::sendMessage(QString topic, QString msg) {
	int rc = MQTTClient_publish(client, topic.toLatin1().data(),
		msg.size() + 1, msg.toLatin1().data(), 0, 0, NULL);
	if (rc != MQTTCLIENT_SUCCESS) {
		fprintf(stderr, "Send failed %d\n", rc);
	} else {
		fprintf(stderr, "Sent\n");
	}
}
