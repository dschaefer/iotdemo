import QtQuick 2.3
import QtQuick.Window 2.2

Window {
    visible: true

    MouseArea {
        anchors.fill: parent
        onClicked: {
            iotClient.sendMessage("sdkTest/msgs", "Hi from QML on Paho");
        }
    }

    Text {
    		id: text
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }
    
    Connections {
    		target: iotClient
    		onMessage: {
    			text.text = msg;
    		}
    }
    
    Component.onCompleted: {
    		iotClient.subscribe("sdkTest/text");
    }
}
