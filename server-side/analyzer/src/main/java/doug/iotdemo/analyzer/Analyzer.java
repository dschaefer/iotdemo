/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package doug.iotdemo.analyzer;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.paho.client.mqttv3.MqttException;

import com.amazonaws.services.dynamodbv2.AmazonDynamoDB;
import com.amazonaws.services.dynamodbv2.AmazonDynamoDBClient;
import com.amazonaws.services.dynamodbv2.model.AttributeAction;
import com.amazonaws.services.dynamodbv2.model.AttributeValue;
import com.amazonaws.services.dynamodbv2.model.AttributeValueUpdate;
import com.amazonaws.services.dynamodbv2.model.ScanRequest;
import com.amazonaws.services.dynamodbv2.model.ScanResult;
import com.amazonaws.services.dynamodbv2.model.UpdateItemRequest;
import com.amazonaws.services.sqs.AmazonSQS;
import com.amazonaws.services.sqs.AmazonSQSClient;
import com.amazonaws.services.sqs.model.Message;
import com.amazonaws.services.sqs.model.ReceiveMessageResult;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import doug.iotdemo.common.AmazonUtils;
import doug.iotdemo.common.MQTTUtils;

public class Analyzer {

	private Map<String, SensorData> data = new HashMap<>();
	private AmazonDynamoDB db = new AmazonDynamoDBClient();
	private MQTTUtils mqtt;

	public static void main(String[] args) {
		try {
			new Analyzer().run();
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	public Analyzer() throws MqttException, IOException {
		Path certPath = Files.createTempFile("cert", ".jks");
		InputStream certIn = getClass().getResourceAsStream("/analyzer.jks");
		Files.copy(certIn, certPath, StandardCopyOption.REPLACE_EXISTING);
		System.setProperty("javax.net.ssl.keyStore", certPath.toString());
		System.setProperty("javax.net.ssl.keyStorePassword", "password");

		mqtt = new MQTTUtils();
	}

	void run() throws IOException, MqttException {
		System.out.println("Doing initial scan");
		doInitialScan();
		System.out.println("Done");

		String url = AmazonUtils.getTimeQueueURL();
		AmazonSQS sqs = new AmazonSQSClient();
		while (true) {
			ReceiveMessageResult msgResult = sqs.receiveMessage(url);
			for (Message msg : msgResult.getMessages()) {
				JsonObject request = new JsonParser().parse(msg.getBody()).getAsJsonObject();
				String sensor = request.get("sensor").getAsString();
				long time = request.get("time").getAsLong();
				addTime(sensor, time);
				saveSensor(sensor);
				sqs.deleteMessage(url, msg.getReceiptHandle());
			}
		}
	}

	void doInitialScan() throws IOException, MqttException {
		// Load up our data
		Map<String, AttributeValue> lastKeyEvaluated = null;
		do {
			ScanRequest request = new ScanRequest().withTableName(AmazonUtils.getTimeTableName())
					.withAttributesToGet("sensor", "time").withExclusiveStartKey(lastKeyEvaluated);
			ScanResult result = db.scan(request);
			for (Map<String, AttributeValue> item : result.getItems()) {
				String sensor = item.get("sensor").getS();
				int time = Integer.valueOf(item.get("time").getN());
				addTime(sensor, time);
				saveSensor(sensor);
			}
			lastKeyEvaluated = result.getLastEvaluatedKey();
		} while (lastKeyEvaluated != null);

		// Save a snapshot for each sensor
		for (String sensor : data.keySet()) {
			saveSensor(sensor);
		}
	}

	void addTime(String sensor, long time) {
		SensorData sensorData = data.get(sensor);
		if (sensorData == null) {
			sensorData = new SensorData();
			data.put(sensor, sensorData);
		}

		sensorData.time += time;
		sensorData.count++;
	}

	void saveSensor(String sensor) throws IOException, MqttException {
		SensorData sensorData = data.get(sensor);
		if (sensorData != null) {
			UpdateItemRequest request = new UpdateItemRequest().withTableName(AmazonUtils.getSensorTableName())
					.addKeyEntry("sensor", new AttributeValue().withS(sensor))
					.addAttributeUpdatesEntry("time",
							new AttributeValueUpdate()
									.withValue(new AttributeValue().withN(Long.toString(sensorData.time)))
									.withAction(AttributeAction.PUT))
					.addAttributeUpdatesEntry("count",
							new AttributeValueUpdate()
									.withValue(new AttributeValue().withN(Long.toString(sensorData.count)))
									.withAction(AttributeAction.PUT));
			db.updateItem(request);

			long thresh = sensorData.time / sensorData.count;
			mqtt.sendThreshold(sensor, thresh);
		}
	}

}
