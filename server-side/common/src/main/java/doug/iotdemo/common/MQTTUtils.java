/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package doug.iotdemo.common;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import com.google.gson.JsonObject;

public class MQTTUtils {

	private MqttClient client;

	public MQTTUtils() throws MqttException, IOException {
		MqttClient client = new MqttClient("ssl://A2KECYFFLC558H.iot.us-east-1.amazonaws.com:8883", "LambdaDevice",
				new MemoryPersistence());

		Path certPath = null;
		try {
			certPath = Files.createTempFile("cert", ".jks");
			InputStream certIn = getClass().getResourceAsStream("/mycert.jks");
			Files.copy(certIn, certPath, StandardCopyOption.REPLACE_EXISTING);
			System.setProperty("javax.net.ssl.keyStore", certPath.toString());
			System.setProperty("javax.net.ssl.keyStorePassword", "password");

			MqttConnectOptions options = new MqttConnectOptions();
			options.setCleanSession(true);
			client.connect(options);
		} finally {
			if (certPath != null) {
				Files.deleteIfExists(certPath);
			}
		}
	}

	public void sendThreshold(String sensor, long thresh) throws IOException, MqttException {
		if (client == null) {
			return;
		}

		JsonObject reply = new JsonObject();
		reply.addProperty("sensor", sensor);
		reply.addProperty("thresh", thresh);

		MqttMessage message = new MqttMessage(reply.toString().getBytes(StandardCharsets.UTF_8));
		client.publish("/factory/in", message);
		client.disconnect();
		System.out.println("Threshold sent");
	}

}
