/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
import QtQuick 2.3
import QtQuick.Window 2.2

Window {
	id: main
	visible: true
	
	Grid {
		id: grid
		columns: 5
		spacing: 5
		anchors.centerIn: parent
		
		SensorButton { sensor: 1; color: "green" }
		SensorButton { sensor: 2; color: "green" }
		SensorButton { sensor: 3; color: "green" }
		SensorButton { sensor: 4; color: "green" }
		SensorButton { sensor: 5; color: "green" }
		SensorButton { sensor: 6; color: "green" }
		SensorButton { sensor: 7; color: "green" }
		SensorButton { sensor: 8; color: "green" }
		SensorButton { sensor: 9; color: "green" }
		SensorButton { sensor: 10; color: "green" }
		SensorButton { sensor: 11; color: "green" }
		SensorButton { sensor: 12; color: "green" }
		SensorButton { sensor: 13; color: "green" }
		SensorButton { sensor: 14; color: "green" }
		SensorButton { sensor: 15; color: "green" }
		SensorButton { sensor: 16; color: "green" }
		SensorButton { sensor: 17; color: "green" }
		SensorButton { sensor: 18; color: "green" }
		SensorButton { sensor: 19; color: "green" }
		SensorButton { sensor: 20; color: "green" }
	}

	Connections {
    		target: manager
    		onStateChanged: {
    			var color;
    			if (state == 0) {
    				color = "green"
    			} else if (state == 1) {
    				color = "yellow"
    			} else if (state == 2) {
    				color = "red"
    			}

    			grid.children[sensor - 1].color = color;
    		}
    }
}

