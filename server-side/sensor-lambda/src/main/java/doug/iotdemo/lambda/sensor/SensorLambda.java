package doug.iotdemo.lambda.sensor;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestStreamHandler;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

public class SensorLambda implements RequestStreamHandler {

	@Override
	public void handleRequest(InputStream input, OutputStream output, Context context) throws IOException {
		JsonObject request = new JsonParser().parse(new InputStreamReader(input)).getAsJsonObject();
		System.out.println("Request: " + request);

		JsonObject reply = new JsonObject();
		if (request.has("init")) {
			reply.addProperty("thresh", 17000);
		}

		// Extract the certificate to tmp
		Path certPath = Files.createTempFile("cert", ".jks");
		InputStream certIn = (getClass().getResourceAsStream("mycert.jks"));
		Files.copy(certIn, certPath);
		System.setProperty("javax.net.ssl.keyStore", certPath.toString());
		System.setProperty("javax.net.ssl.keyStorePassword", "password");

		// Post the message back to MQTT server
		try {
			MqttClient client = new MqttClient("ssl://A2KECYFFLC558H.iot.us-east-1.amazonaws.com:8883", "LambdaDevice");
			MqttConnectOptions options = new MqttConnectOptions();
			options.setCleanSession(true);
			client.connect(options);

			MqttMessage message = new MqttMessage(reply.toString().getBytes(StandardCharsets.UTF_8));
			client.publish("/factory/in", message);
			client.disconnect();
		} catch (MqttException e) {
			throw new IOException(e);
		}
	}

}
