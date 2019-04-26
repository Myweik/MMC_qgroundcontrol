import QtQuick 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0

Item {
    width:column_.width + 20
    height: column_.height + 20

    MMCMessageDialog{
        id: dropSet_
        dialogType: 0
        width: 240
        labelStr: qsTr("Preset throw weight settings(g)", "预设抛投重量(g)")
        titleStr: qsTr("Weight Setting")
        onAcceptWindow:{
            _activeVehicle.currentMount.weightControl(parseInt(text))
        }
    }
    //智能抛投
   Column{
       id: column_
        anchors.centerIn: parent
        spacing: 10
        Row{
            spacing: 30
            Row{
                QGCLabel {
                    font.pointSize: 10
                    text:  qsTr("Current Weight:", "当前重量:")
                }
                QGCLabel {
                    font.pointSize: 10
                    text:  mountIsActive&& _activeVehicle.currentMount.mountType === 298?_activeVehicle.currentMount.currentWeight*10 + "g":""
                }
            }
            Row{
                QGCLabel {
                    font.pointSize: 10
                    text:  qsTr("Max Weight:", "最大重量:")
                }
                QGCLabel {
                    font.pointSize: 10
                    text:  mountIsActive&& _activeVehicle.currentMount.mountType === 298?_activeVehicle.currentMount.weight+ "0g":""
                }
            }
            Row{
                QGCLabel {
                    font.pointSize: 10
                    text:  qsTr("State:", "状态:")
                }
                QGCLabel {
                    font.pointSize: 10
                    text:  mountIsActive&& _activeVehicle.currentMount.mountType === 298?_activeVehicle.currentMount.state:""
                }
            }

            Row{
                QGCLabel {
                    font.pointSize: 10
                    text:  qsTr("Mode:", "模式:")
                }
                QGCLabel {
                    font.pointSize: 10
                    text:  mountIsActive&& _activeVehicle.currentMount.mountType === 298?_activeVehicle.currentMount.mode:""
                }
            }
        }

        Row{
            spacing: 5
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
                                if(mountIsActive)
                                    _activeVehicle.currentMount.pitchControl(speed_control2.currentIndex+1, true)
                            }
                            Timer{
                                interval: 100
                                repeat: true
                                running: parent.longPreseed
                                onTriggered: {
                                    _activeVehicle.currentMount.pitchControl(speed_control2.currentIndex+1, true)
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
                                if(mountIsActive)
                                    _activeVehicle.currentMount.pitchControl(speed_control2.currentIndex+1, false)
                            }
                            Timer{
                                interval: 100
                                repeat: true
                                running: parent.longPreseed
                                onTriggered: {
                                    _activeVehicle.currentMount.pitchControl(speed_control2.currentIndex+1, false)
                                }
                            }
                        }

                        Row{
                            anchors.horizontalCenter: parent.horizontalCenter
                            QGCLabel {
                                anchors.verticalCenter: parent.verticalCenter
                                font.pointSize: 10
                                text:  qsTr("Speed：", "控制速度:")
                            }
                            MMCComboBox {
                                id: speed_control2
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
                        spacing: 30
                        Row{
                            anchors.horizontalCenter: parent.horizontalCenter
                            spacing: 20
                            QGCLabel{
                                text: qsTr("Mode:", "智能模式:")
                            }
                            MMCSwitch{
                                checked: (mountIsActive && _activeVehicle.currentMount.mountType===298)?_activeVehicle.currentMount.mode===1:false
                                onCheckedChanged:{
                                    if(mountIsActive)
                                    {
                                        if(_activeVehicle.currentMount.mode < 0)
                                            return;
                                        if(checked)
                                        {
                                            _activeVehicle.currentMount.modeControl(1)
                                        }
                                        else{
                                            _activeVehicle.currentMount.modeControl(0)
                                        }
                                    }
                                }
                            }
                        }
                        Item{
                            height:1
                        }

                        Rectangle{
                            id: delayBtn_
                            property int step: 0
                            property bool hovered: false
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: 80
                            height: 80
                            radius: 40
                            border.color: delayBtn_.hovered?"#32ae47":"white"
                            border.width: 2
                            color: "#3e3e3e"
                            QGCLabel{
                                anchors.centerIn: parent
                                text: qsTr("Drop", "抛下")
                                color: delayBtn_.hovered?"#32ae47":"white"
                            }
                            MouseArea{
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton
                                hoverEnabled: true
                                onClicked: {
                                    delayBtn_.visible = false
                                    dumpConfirm.visible = true
                                }
                                onEntered: parent.hovered = true
                                onExited: parent.hovered = false
                            }
                        }

                        SliderSwitch {
                            id:   dumpConfirm
                            anchors.horizontalCenter: parent.horizontalCenter
                            visible:                    false
                            z:                          QGroundControl.zOrderWidgets
                            fontPointSize:              _guidedModeBar._fontPointSize
                            confirmText:        qsTr("Drop", "抛下")
                            onAccept: {
                                delayBtn_.visible = true
                                dumpConfirm.visible = false
                                _activeVehicle.currentMount.dropCmmd()
                            }
                            onReject: {
                                delayBtn_.visible = true
                                dumpConfirm.visible = false
                            }
                        }
                        Item{
                            height:30
                        }

                        QGCButton {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr("PreSet Weight:", "更改预设重量")
                            width: 160
                            _radius:4
                            onDelayClick: {
                                dropSet_.show()
                            }
                        }
                        Item{
                            height: 5
                        }

                        QGCLabel{
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: "#00cc00"
                            text: {
                                if(mountIsActive)
                                {
                                    switch(_activeVehicle.currentMount.state)
                                    {
                                    case 1:
                                        return qsTr("The bolt has opened", "抛投已打开");
                                    }
                                }
                                return ""
                            }
                        }
                    }
                }
            }
            Column{
                anchors.verticalCenter: parent.verticalCenter
                Image{
                    source:{
                        return "/qmlimages/dump.png";
                    }
                }
                QGCLabel{
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: {
                        if(mountIsActive)
                        {
                            return _activeVehicle.currentMount.mountName(_activeVehicle.currentMount.mountType)
                        }
                        return ""
                    }
                }
                QGCLabel{
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: _activeVehicle && !_activeVehicle.mountLost? _activeVehicle.currentMount.version:""
                }
            }
        }
    }

}

