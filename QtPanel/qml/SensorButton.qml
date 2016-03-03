/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
import QtQuick 2.0

Item {
	id: container
	property int sensor
	property alias color: rectangle.color

	width: 100
	height: 100
	
	Rectangle {
		id: rectangle
		anchors.fill: parent

	    Text {
	    	id: text
	        text: sensor
	        anchors.centerIn: parent
        }
    }

	MouseArea {
		anchors.fill: parent
		onPressed: manager.setState(sensor, 1);
		onReleased: manager.setState(sensor, 0);
	}
}
