import QtQuick 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0

Item {
    property bool _theMountIsActive: mountIsActive && _activeVehicle.currentMount.mountType===266

    width: row_.width + 20
    height: row_.height + 20
    Row{
        id: row_
        anchors.centerIn: parent
        spacing: 10
        Rectangle{
            height: 290
            width: 188
            color: "#282828"
            Rectangle{
                anchors.fill: parent
                anchors.margins: 1
                border.color: "#4e4d4d"
                color: "#3e3e3e"
                Column{
                    anchors.centerIn: parent
                    spacing: 40
                    MMCImageButton {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 50
                        height: 50
                        sourceWidth: 50
                        sourceHeight: 50
                        checkable: false
                        source:    pressed?"qrc:/qmlimages/rotation_press.svg":"qrc:/qmlimages/rotation.svg"
                        backColor: "transparent"
                        overColor: false
                        onPressedChanged:{
                            if(_theMountIsActive)
                                _activeVehicle.currentMount.pitchControl(speed_control3.currentIndex+1, true)
                        }
                        Timer{
                            interval: 100
                            repeat: true
                            running: parent.longPreseed
                            onTriggered: {
                                _activeVehicle.currentMount.pitchControl(speed_control3.currentIndex+1, true)
                            }
                        }
                    }

                    MMCImageButton {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 50
                        height: 50
                        sourceWidth: 50
                        sourceHeight: 50
                        checkable: false
                        rotation: 180
                        source:    pressed?"qrc:/qmlimages/rotation_press.svg":"qrc:/qmlimages/rotation.svg"
                        backColor: "transparent"
                        overColor: false
                        onPressedChanged:{
                            if(_theMountIsActive)
                                _activeVehicle.currentMount.pitchControl(speed_control3.currentIndex+1, false)
                        }
                        Timer{
                            interval: 100
                            repeat: true
                            running: parent.longPreseed
                            onTriggered: {
                                _activeVehicle.currentMount.pitchControl(speed_control3.currentIndex+1, false)
                            }
                        }
                    }

                    Row{
                        anchors.horizontalCenter: parent.horizontalCenter
                        QGCLabel {
                            anchors.verticalCenter: parent.verticalCenter
                            font.pointSize: 10
                            text:  qsTr("Control Speed：", "控制速度:")
                        }
                        MMCComboBox {
                            id: speed_control3
                            width: 80
                            height: 30
                            elideWidth: 60
                            currentIndex: 2
                            model: ["x1", "x2", "x3", "x4", "x5"]
                        }
                    }
                }
            }
        }
        Rectangle{
            height: 290
            width: 240
            color: "#282828"
            Rectangle{
                anchors.fill: parent
                anchors.margins: 1
                border.color: "#4e4d4d"
                color: "#3e3e3e"
                Column{
                    anchors.centerIn: parent
                    spacing: 20
                    Row{
                        QGCLabel {
                            anchors.verticalCenter: parent.verticalCenter
                            font.pointSize: 10
                            text:  qsTr("Volume：", "音量:")
                        }
                        QGCLabel{
                            anchors.verticalCenter: parent.verticalCenter
                            text: (_theMountIsActive)?_activeVehicle.currentMount.volume:""
                        }
                    }
                    MMCSliderBar{
                        autoReturnCenter: false
                        stepSize: 1
                        minimumValue: 200
                        maximumValue: 230
                        enabled: control_volumn.checked
                        value: _theMountIsActive ?_activeVehicle.currentMount.volume:200
                        onValueChanged: {
                            if(_theMountIsActive  && control_volumn.checked && value !== _activeVehicle.currentMount.volume && value >= 200)
                                _activeVehicle.currentMount.volumeControl(value)
                        }
                    }

                    Row{
                        spacing: 10
                        QGCLabel {
                            anchors.verticalCenter: parent.verticalCenter
                            font.pointSize: 10
                            text:  qsTr("controlEnable:", "控制使能:")
                        }

                        MMCSwitch{
                            anchors.verticalCenter: parent.verticalCenter
                            id: control_volumn
                        }
                    }
                }
            }
        }
        Column{
            anchors.verticalCenter: parent.verticalCenter
            Image{
                source:{
                    return "/qmlimages/loudSpeaker.png";
                }
            }
            QGCLabel{
                anchors.horizontalCenter: parent.horizontalCenter
                text: {
                    if(_theMountIsActive)
                    {
                        return _activeVehicle.currentMount.mountName(_activeVehicle.currentMount.mountType)
                    }
                    return ""
                }
            }
            QGCLabel{
                anchors.horizontalCenter: parent.horizontalCenter
                text: _theMountIsActive? _activeVehicle.currentMount.version:""
            }
        }
//        Component.onCompleted: {
//            isCompleted = true;
//        }
    }
}

