/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package doug.iotdemo.common;

import com.amazonaws.services.cloudformation.AmazonCloudFormation;
import com.amazonaws.services.cloudformation.AmazonCloudFormationClient;
import com.amazonaws.services.cloudformation.model.DescribeStacksRequest;
import com.amazonaws.services.cloudformation.model.Output;
import com.amazonaws.services.cloudformation.model.Stack;

public class AmazonUtils {

	private static final String stackName = "Doug-IoTDemo";
	private static String timeQueueName = "TimeQueueName";
	private static String sensorTableName = "SensorTableName";

	public static String getStackOutput(String outputKey) {
		AmazonCloudFormation cf = new AmazonCloudFormationClient();
		DescribeStacksRequest request = new DescribeStacksRequest().withStackName(stackName);
		for (Stack stack : cf.describeStacks(request).getStacks()) {
			for (Output output : stack.getOutputs()) {
				if (outputKey.equals(output.getOutputKey())) {
					return output.getOutputValue();
				}
			}
		}
		return null;
	}

	public static String getTimeQueueURL() {
		return getStackOutput(timeQueueName);
	}

	public static String getSensorTableName() {
		return getStackOutput(sensorTableName);
	}

	public static String getTimeTableName() {
		return getStackOutput(timeQueueName);
	}

}
