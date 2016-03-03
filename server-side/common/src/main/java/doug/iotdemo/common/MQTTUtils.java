/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package doug.iotdemo.common;

import java.io.IOException;
import java.nio.charset.StandardCharsets;

import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import com.google.gson.JsonObject;

public class MQTTUtils {

	private final MqttClient client;

	public MQTTUtils() throws MqttException, IOException {
		String url = "ssl://A2KECYFFLC558H.iot.us-east-1.amazonaws.com:8883";
		if (System.getProperty("mqtt.url") != null) {
			url = System.getProperty("mqtt.url");
		}
		client = new MqttClient(url, "LambdaDevice", new MemoryPersistence());
		MqttConnectOptions options = new MqttConnectOptions();
		options.setCleanSession(true);
		client.connect(options);
		System.out.println("Connected to " + url);
	}

	public void sendThreshold(String sensor, long thresh) throws MqttException {
		if (!client.isConnected()) {
			client.connect();
		}

		JsonObject reply = new JsonObject();
		reply.addProperty("sensor", sensor);
		reply.addProperty("thresh", thresh);

		MqttMessage message = new MqttMessage(reply.toString().getBytes(StandardCharsets.UTF_8));
		client.publish("/factory/in", message);
	}

}
