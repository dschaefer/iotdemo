package doug.iotdemo.lambda.sensor;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.paho.client.mqttv3.MqttException;

import com.amazonaws.services.dynamodbv2.AmazonDynamoDB;
import com.amazonaws.services.dynamodbv2.AmazonDynamoDBClient;
import com.amazonaws.services.dynamodbv2.model.AttributeAction;
import com.amazonaws.services.dynamodbv2.model.AttributeValue;
import com.amazonaws.services.dynamodbv2.model.AttributeValueUpdate;
import com.amazonaws.services.dynamodbv2.model.GetItemRequest;
import com.amazonaws.services.dynamodbv2.model.GetItemResult;
import com.amazonaws.services.dynamodbv2.model.UpdateItemRequest;
import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestStreamHandler;
import com.amazonaws.services.sqs.AmazonSQS;
import com.amazonaws.services.sqs.AmazonSQSClient;
import com.amazonaws.services.sqs.model.SendMessageRequest;
import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import doug.iotdemo.common.AmazonUtils;
import doug.iotdemo.common.MQTTUtils;

public class SensorLambda implements RequestStreamHandler {

	private AmazonDynamoDB db = new AmazonDynamoDBClient();
	private String sensorTableName = AmazonUtils.getSensorTableName();

	@Override
	public void handleRequest(InputStream input, OutputStream output, Context context) throws IOException {
		JsonObject request = new JsonParser().parse(new InputStreamReader(input)).getAsJsonObject();
		System.out.println("Request: " + request);

		if (!request.has("sensor")) {
			System.out.println("No sensor id");
			return;
		}

		if (request.has("init")) {
			handleInitMessage(request);
		}

		if (request.has("state")) {
			handleStateChange(request);
		}

		if (request.has("time")) {
			handleTime(request);
		}
	}

	/*
	 * Send down the current threshold value.
	 */
	private void handleInitMessage(JsonObject request) throws IOException {
		try {
			long thresh = 20000; // default

			String sensor = request.get("sensor").getAsString();
			GetItemRequest getRequest = new GetItemRequest().withTableName(sensorTableName).addKeyEntry("sensor",
					new AttributeValue().withS(sensor));
			GetItemResult getResult = db.getItem(getRequest);
			Map<String, AttributeValue> sensorItem = getResult.getItem();
			if (sensorItem != null) {
				long time = Long.parseLong(sensorItem.get("time").getN());
				long count = Long.parseLong(sensorItem.get("count").getN());
				thresh = time / count;
			}

			new MQTTUtils().sendThreshold(sensor, thresh);
		} catch (MqttException e) {
			throw new IOException(e);
		}
	}

	/*
	 * Update the sensor's state in the db.
	 */
	private void handleStateChange(JsonObject request) throws IOException {
		UpdateItemRequest updateRequest = new UpdateItemRequest().withTableName(sensorTableName)
				.addKeyEntry("sensor", new AttributeValue().withS(request.get("sensor").getAsString()))
				.addAttributeUpdatesEntry("state",
						new AttributeValueUpdate(
								new AttributeValue().withN(Integer.toString(request.get("state").getAsInt())),
								AttributeAction.PUT));
		db.updateItem(updateRequest);
		System.out.println("Sensor table updated");
	}

	/*
	 * Store the new time value in Dynamo and send SQS to analyzer
	 */
	private void handleTime(JsonObject request) throws IOException {
		Map<String, AttributeValue> item = new HashMap<>();
		item.put("sensor", new AttributeValue().withS(request.get("sensor").getAsString()));
		item.put("timestamp", new AttributeValue().withN(Long.toString(System.currentTimeMillis())));
		item.put("time", new AttributeValue().withN(Integer.toString(request.get("time").getAsInt())));
		db.putItem(AmazonUtils.getTimeTableName(), item);
		System.out.println("Item writen to TimeTable");

		AmazonSQS sqs = new AmazonSQSClient();
		String url = AmazonUtils.getTimeQueueURL();
		sqs.sendMessage(new SendMessageRequest(url, new Gson().toJson(request)));
		System.out.println("SQS Message sent to " + url);
	}

}
