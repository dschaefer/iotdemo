/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
#include <src/Manager.h>

Manager::Manager(QObject * parent)
: QObject(parent), iotClient("ssl://192.168.42.1:8883", "BeagleBone", this), sensorReceiver(this)
{
	iotClient.connect();
}
