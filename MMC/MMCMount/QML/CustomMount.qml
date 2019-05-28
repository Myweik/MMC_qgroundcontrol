import QtQuick 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import MMCSettings 1.0

Item {
    width:_customMount.width + 20
    height: _customMount.height + 20
    Row {
        id: _customMount
        anchors.centerIn: parent
        spacing: 10

        property string channelName: qsTr("Channel ", "通道")

        property int channel1Value: 1015
        property int channel2Value: 1015
        property int channel3Value: 1015
        property int channel4Value: 1015
        property int channel5Value: 1015
        property int channel6Value: 1015
        property int channel7Value: 1015
        property int channel8Value: 1015
        property int virtualCommand: MMCSettings.dValue("virtualCommand",0,"USER_DEFINE_CONFIG")
        onVirtualCommandChanged: {
            MMCSettings.dSetValue("virtualCommand",virtualCommand,"USER_DEFINE_CONFIG");
        }

        Timer {
            id: collection_
            interval:   100
            running:   _activeVehicle ? true : false
            repeat:     true
            onTriggered: {
                if (_activeVehicle && (_activeVehicle.isControlMount !== 1) && win_.visibility) {
                    _activeVehicle.virtualMountControl(_customMount.channel1Value.toFixed(0),
                                                      _customMount.channel2Value.toFixed(0),
                                                      _customMount.channel3Value.toFixed(0),
                                                      _customMount.channel4Value.toFixed(0),
                                                      _customMount.channel5Value.toFixed(0),
                                                      _customMount.channel6Value.toFixed(0),
                                                      _customMount.channel7Value.toFixed(0),
                                                      _customMount.channel8Value.toFixed(0));
                }
            }
        }

        Timer{
            id: delayTimer_
            interval: 200
            running: false
            onTriggered: {
                collection_.interval = 200
            }
        }

        Rectangle{
            height: 290
            width: 150
            color: "#282828"
            Rectangle{
                anchors.fill: parent
                anchors.margins: 1
                border.color: "#4e4d4d"
                color: "#3e3e3e"
                Column{
                    anchors.centerIn: parent
                    spacing: 15
                    Row{
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.horizontalCenterOffset: -20

                        QGCLabel{
                            id:channel3Label
                            anchors.verticalCenter: parent.verticalCenter
                            font.pointSize: 13
                            text: _customMount.channelName + "3"
                        }
                        MMCSliderBar{
                            id: channel3
                            orientation: Qt.Vertical
                            value: 1523
                            stepSize:101
                            minimumValue: 1015
                            maximumValue: 2030
                            onValueChanged:{
//                                console.log( "channel3",channel3.value)
                                if(value < 1560 && value > 1450)
                                {
                                    _customMount.channel3Value = 1523

                                }
                                else
                                    _customMount.channel3Value =  value
                                _customMount.valueChanged()
                            }
                            onPressedChanged:  {
                                if(!pressed && autoReturnCenter)
                                {
                                    value = 1523
                                }
                            }
                        }
                    }
                    Column{
                        anchors.horizontalCenter: parent.horizontalCenter
                        MMCSliderBar{
                            id: channel1
                            width: 136
                            minimumValue: 1015
                            maximumValue: 2030
                            value: 1523
                            onValueChanged:{
                                _customMount.channel1Value = -value + 3045
                                _customMount.valueChanged()
                            }
                            onPressedChanged:  {
                                if(!pressed && autoReturnCenter)
                                {
                                    value = 1523
                                }
                            }
                        }
                        QGCLabel{
                            id:channel1Label
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.pointSize: 13
                            text: _customMount.channelName + "1"
                        }
                    }
                }
            }
        }

        Rectangle{
            height: 290
            width: 150
            color: "#282828"
            Rectangle{
                anchors.fill: parent
                anchors.margins: 1
                border.color: "#4e4d4d"
                color: "#3e3e3e"
                Column{
                    anchors.centerIn: parent
                    spacing: 5
                    Row{
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.horizontalCenterOffset: -20
                        QGCLabel{
                            id:channel2Label
                            anchors.verticalCenter: parent.verticalCenter
                            font.pointSize: 13
                            text: _customMount.channelName + "2"
                        }
                        MMCSliderBar{
                            id: channel2
                            orientation: Qt.Vertical
                            value: 1523
                            minimumValue: 1015
                            maximumValue: 2030
                            onValueChanged:{
                                _customMount.channel2Value = value
                                _customMount.valueChanged()
                            }
                            onPressedChanged:  {
                                if(!pressed && autoReturnCenter)
                                {
                                    value = 1523
                                }
                            }
                        }
                    }
                    Column{
                        anchors.horizontalCenter: parent.horizontalCenter
                        MMCSliderBar{
                            id: channel4
                            width: 136
                            value: 1523
                            minimumValue: 1015
                            maximumValue: 2030
                            onValueChanged:{
                                _customMount.channel4Value = -value + 3045
                                _customMount.valueChanged()
                            }
                            onPressedChanged:  {
                                if(!pressed && autoReturnCenter)
                                {
                                    value = 1523
                                }
                            }
                        }
                        QGCLabel{
                            id:channel4Label
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.pointSize: 13
                            text: _customMount.channelName + "4"
                        }
                    }
                }
            }
        }

        Rectangle{
            height: 290
            width: 150
            color: "#282828"
            Rectangle{
                anchors.fill: parent
                anchors.margins: 1
                border.color: "#4e4d4d"
                color: "#3e3e3e"
                Column{
                    anchors.centerIn: parent
                    spacing: 10
                    Column{
                        Image{
                            anchors.left: parent.left
                            width: parent.width
                            //anchors.right: parent.right
                            source: "/qmlimages/switch3.svg"
                        }
                        Item { width: 1;  height:10 }
                        Column{
                            //spacing: 5
                            //anchors.horizontalCenter: parent.horizontalCenter
                            MMCComboBox {
                                id: combox1
                                fontSize: 13
                                width: 130
                                height: 20
                                model: [ _customMount.channelName + "5", _customMount.channelName + "6",  _customMount.channelName + "7",  _customMount.channelName + "8"]
                                onCurrentIndexChanged:{
                                    if(combox1.currentIndex === 0)
                                        _customMount.channel5Value = switch1.value
                                    else if(combox1.currentIndex === 1)
                                        _customMount.channel6Value = switch1.value
                                    else if(combox1.currentIndex === 2)
                                        _customMount.channel7Value = switch1.value
                                    else if(combox1.currentIndex === 3)
                                        _customMount.channel8Value = switch1.value
                                }
                            }
                            MMCSliderBar{
                                id: switch1
                                width: 130
                                height: 30
                                // anchors.horizontalCenter: parent.horizontalCenter
                                minimumValue: 1015
                                maximumValue: 2030
                                autoReturnCenter: false
                                stepSize: 508
                                isSwidth: true
                                onValueChanged:{
                                    if(combox1.currentIndex === 0)
                                        _customMount.channel5Value = value
                                    else if(combox1.currentIndex === 1)
                                        _customMount.channel6Value = value
                                    else if(combox1.currentIndex === 2)
                                        _customMount.channel7Value = value
                                    else if(combox1.currentIndex === 3)
                                        _customMount.channel8Value = value
                                    _customMount.valueChanged()
                                }
                            }
                        }
                        Item { width: 1;  height:10 }
                        Column{
                            //spacing: 5
                            MMCComboBox {
                                id: combox2
                                width: 130
                                height: 20
                                currentIndex:2
                                fontSize: 13
                                model: [ _customMount.channelName + "5", _customMount.channelName + "6",  _customMount.channelName + "7",  _customMount.channelName + "8"]
                                onCurrentIndexChanged:{
                                    if(switch2 != null)
                                    {
                                        if(combox2.currentIndex === 0 && combox1.currentIndex !== 0)
                                            _customMount.channel5Value = switch2.value
                                        else if(combox2.currentIndex === 1 && combox1.currentIndex !== 1)
                                            _customMount.channel6Value = switch2.value
                                        else if(combox2.currentIndex === 2 && combox1.currentIndex !== 2)
                                            _customMount.channel7Value = switch2.value
                                        else if(combox2.currentIndex === 3 && combox1.currentIndex !== 3)
                                            _customMount.channel8Value = switch2.value
                                    }
                                }
                            }
                            MMCSliderBar{
                                width: 130
                                height: 30
                                //anchors.horizontalCenter: parent.horizontalCenter
                                id: switch2
                                minimumValue: 1015
                                maximumValue: 2030
                                autoReturnCenter: false
                                stepSize: 508
                                isSwidth: true
                                onValueChanged:{
                                    if(combox2.currentIndex === 0 && combox1.currentIndex !== 0)
                                        _customMount.channel5Value = switch2.value
                                    else if(combox2.currentIndex === 1 && combox1.currentIndex !== 1)
                                        _customMount.channel6Value = switch2.value
                                    else if(combox2.currentIndex === 2 && combox1.currentIndex !== 2)
                                        _customMount.channel7Value = switch2.value
                                    else if(combox2.currentIndex === 3 && combox1.currentIndex !== 3)
                                        _customMount.channel8Value = switch2.value
                                    _customMount.valueChanged()
                                }
                            }
                        }
                        // Item { width: 1;  height:10 }
                    }
                    Column{
                        spacing: 5
                        MMCComboBox {
                            id: combox3
                            width: 130
                            height: 20
                            currentIndex:1
                            fontSize: 13
                            model: [ _customMount.channelName + "5", _customMount.channelName + "6",  _customMount.channelName + "7",  _customMount.channelName + "8"]
                            onCurrentIndexChanged:{
                                if(switch3 != null)
                                {
                                    if(combox3.currentIndex === 0 &&
                                            combox2.currentIndex !== 0 && combox1.currentIndex !== 0)
                                        _customMount.channel5Value = switch3.value
                                    else if(combox3.currentIndex === 1 &&
                                            combox2.currentIndex !== 1 && combox1.currentIndex !== 1)
                                        _customMount.channel6Value = switch3.value
                                    else if(combox3.currentIndex === 2 &&
                                            combox2.currentIndex !== 2 && combox1.currentIndex !== 2)
                                        _customMount.channel7Value = switch3.value
                                    else if(combox3.currentIndex === 3 &&
                                            combox2.currentIndex !== 3 && combox1.currentIndex !== 3)
                                        _customMount.channel8Value = switch3.value
                                }
                            }
                        }
                        MMCSwitch{
                            id: switch3
                            // Layout.leftMargin: 28
                            property int value: 1015
                            onCheckedChanged:{
                                if(checked)
                                    value = 2030
                                else
                                    value = 1015
                                if(combox3.currentIndex === 0 &&
                                        combox2.currentIndex !== 0 && combox1.currentIndex !== 0)
                                    _customMount.channel5Value = switch3.value
                                else if(combox3.currentIndex === 1 &&
                                        combox2.currentIndex !== 1 && combox1.currentIndex !== 1)
                                    _customMount.channel6Value = switch3.value
                                else if(combox3.currentIndex === 2 &&
                                        combox2.currentIndex !== 2 && combox1.currentIndex !== 2)
                                    _customMount.channel7Value = switch3.value
                                else if(combox3.currentIndex === 3 &&
                                        combox2.currentIndex !== 3 && combox1.currentIndex !== 3)
                                    _customMount.channel8Value = switch3.value
                                _customMount.valueChanged()
                            }
                        }
                    }
                    Column{
                        spacing: 5
                        MMCComboBox {
                            id: combox4
                            width: 130
                            height: 20
                            currentIndex:3
                            fontSize: 13
                            model: [ _customMount.channelName + "5", _customMount.channelName + "6",  _customMount.channelName + "7",  _customMount.channelName + "8"]
                            onCurrentIndexChanged:{
                                if(switch4 != null)
                                {
                                    if(combox4.currentIndex === 0 &&
                                            combox3.currentIndex !== 0 && combox2.currentIndex !== 0 && combox1.currentIndex !== 0)
                                        _customMount.channel5Value = switch4.value
                                    else if(combox4.currentIndex === 1 &&
                                            combox3.currentIndex !== 1 && combox2.currentIndex !== 1 && combox1.currentIndex !== 1)
                                        _customMount.channel6Value = switch4.value
                                    else if(combox4.currentIndex === 2 &&
                                            combox3.currentIndex !== 2 && combox2.currentIndex !== 2 && combox1.currentIndex !== 2)
                                        _customMount.channel7Value = switch4.value
                                    else if(combox4.currentIndex === 3 &&
                                            combox3.currentIndex !== 3 && combox2.currentIndex !== 3 && combox1.currentIndex !== 3)
                                        _customMount.channel8Value = switch4.value
                                }
                            }
                        }
                        MMCSwitch{
                            id: switch4
                            property int value: 1015
                            onCheckedChanged:{
                                if(checked)
                                    value = 2030
                                else
                                    value = 1015
                                if(combox4.currentIndex === 0 &&
                                        combox3.currentIndex !== 0 && combox2.currentIndex !== 0 && combox1.currentIndex !== 0)
                                    _customMount.channel5Value = switch4.value
                                else if(combox4.currentIndex === 1 &&
                                        combox3.currentIndex !== 1 && combox2.currentIndex !== 1 && combox1.currentIndex !== 1)
                                    _customMount.channel6Value = switch4.value
                                else if(combox4.currentIndex === 2 &&
                                        combox3.currentIndex !== 2 && combox2.currentIndex !== 2 && combox1.currentIndex !== 2)
                                    _customMount.channel7Value = switch4.value
                                else if(combox4.currentIndex === 3 &&
                                        combox3.currentIndex !== 3 && combox2.currentIndex !== 3 && combox1.currentIndex !== 3)
                                    _customMount.channel8Value = switch4.value
                                _customMount.valueChanged()
                            }
                        }
                    }
                }
            }
        }

        Column{
            anchors.verticalCenter: parent.verticalCenter
            Image{
                source: "/qmlimages/customMount.png"
            }
        }
        Component.onCompleted: {
            isCompleted = true;
        }

        function valueChanged(){
            if(_activeVehicle && (_activeVehicle.isControlMount !== 1) && mountWindowActive && win_.visibility)
            {
                _activeVehicle.virtualMountControl(_customMount.channel1Value.toFixed(0),
                                                  _customMount.channel2Value.toFixed(0),
                                                  _customMount.channel3Value.toFixed(0),
                                                  _customMount.channel4Value.toFixed(0),
                                                  _customMount.channel5Value.toFixed(0),
                                                  _customMount.channel6Value.toFixed(0),
                                                  _customMount.channel7Value.toFixed(0),
                                                  _customMount.channel8Value.toFixed(0));
            }
            if(collection_.interval != 50)
                collection_.interval = 50
            if(delayTimer_.running)
                delayTimer_.restart()
            else
                delayTimer_.start()
        }
    }
}

