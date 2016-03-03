/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package doug.iotdemo.server.web;

import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.util.Map;

import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttAsyncClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import com.amazonaws.handlers.AsyncHandler;
import com.amazonaws.services.dynamodbv2.AmazonDynamoDBAsync;
import com.amazonaws.services.dynamodbv2.AmazonDynamoDBAsyncClient;
import com.amazonaws.services.dynamodbv2.model.AttributeValue;
import com.amazonaws.services.dynamodbv2.model.ScanRequest;
import com.amazonaws.services.dynamodbv2.model.ScanResult;

import doug.iotdemo.common.AmazonUtils;
import io.vertx.core.Vertx;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.StaticHandler;

public class WebServer {

	private AmazonDynamoDBAsync db = new AmazonDynamoDBAsyncClient();
	private String sensorTableName = AmazonUtils.getSensorTableName();
	private MqttAsyncClient mqtt;

	public static void main(String[] args) throws Throwable {
		Path certPath = Files.createTempFile("cert", ".jks");
		InputStream certIn = WebServer.class.getResourceAsStream("/server.jks");
		Files.copy(certIn, certPath, StandardCopyOption.REPLACE_EXISTING);
		System.setProperty("javax.net.ssl.keyStore", certPath.toString());
		System.setProperty("javax.net.ssl.keyStorePassword", "password");

		new WebServer().run();
	}

	private void run() throws MqttException, IOException {
		// MQTT
		// String url = "ssl://A2KECYFFLC558H.iot.us-east-1.amazonaws.com:8883";
		String url = "ssl://localhost:8883";
		mqtt = new MqttAsyncClient(url, "LambdaDevice", new MemoryPersistence());
		MqttConnectOptions options = new MqttConnectOptions();
		options.setKeepAliveInterval(20);
		options.setCleanSession(true);
		mqtt.connect(options);

		// Vertx
		Vertx vertx = Vertx.factory.vertx();

		Router router = Router.router(vertx);

		// router.route().handler(LoggerHandler.create(LoggerFormat.DEFAULT));

		router.get("/api/sensors").handler(this::handleFetch);
		router.put("/api/sensors").handler(this::handlePut);

		router.route().handler(StaticHandler.create("www").setCachingEnabled(false));

		vertx.createHttpServer().requestHandler(router::accept).listen(8080);
		System.out.println("Listening on port 8080");
	}

	private void handleFetch(RoutingContext context) {
		ScanRequest scanRequest = new ScanRequest().withTableName(sensorTableName).withAttributesToGet("sensor",
				"state");
		db.scanAsync(scanRequest, new AsyncHandler<ScanRequest, ScanResult>() {
			@Override
			public void onSuccess(ScanRequest request, ScanResult result) {
				JsonObject states = new JsonObject();
				for (Map<String, AttributeValue> item : result.getItems()) {
					AttributeValue stateValue = item.get("state");
					if (stateValue != null) {
						JsonObject sensorObj = new JsonObject();
						sensorObj.put("state", Integer.valueOf(stateValue.getN()));
						states.put(item.get("sensor").getS(), sensorObj);
					}
				}
				context.response().end(states.encode());
			}

			@Override
			public void onError(Exception exception) {
				StringWriter msg = new StringWriter();
				exception.printStackTrace(new PrintWriter(msg));
				context.response().setStatusCode(500).end(msg.toString());
			}
		});
	}

	private void handlePut(RoutingContext context) {
		JsonObject msg = new JsonObject();
		msg.put("sensor", context.request().getParam("sensor"));
		msg.put("state", Integer.valueOf(context.request().getParam("state")));

		MqttMessage message = new MqttMessage(msg.toString().getBytes(StandardCharsets.UTF_8));
		try {
			if (!mqtt.isConnected()) {
				mqtt.connect(this, new IMqttActionListener() {
					@Override
					public void onSuccess(IMqttToken arg0) {
						try {
							publishMessage(message, context);
						} catch (MqttException e) {
							throw new RuntimeException(e);
						}
					}

					@Override
					public void onFailure(IMqttToken arg0, Throwable t) {
						StringWriter msg = new StringWriter();
						t.printStackTrace(new PrintWriter(msg));
						context.response().setStatusCode(500).end(msg.toString());
					}
				});
			} else {
				publishMessage(message, context);
			}
		} catch (MqttException e) {
			throw new RuntimeException(e);
		}
	}

	private void publishMessage(MqttMessage message, RoutingContext context) throws MqttException {
		mqtt.publish("/factory/in", message, this, new IMqttActionListener() {
			@Override
			public void onSuccess(IMqttToken arg0) {
				context.response().end();
			}

			@Override
			public void onFailure(IMqttToken arg0, Throwable t) {
				StringWriter msg = new StringWriter();
				t.printStackTrace(new PrintWriter(msg));
				context.response().setStatusCode(500).end(msg.toString());
			}
		});
	}
}
