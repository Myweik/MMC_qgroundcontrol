import QtQuick 2.5
import QtQuick.Layouts 1.2
import QGroundControl.Controls 1.0
import QtQuick.Controls 1.4


Item {
    width:   mainShow.width
    height: mainShow.height

    property bool _theMountIsActive: mountIsActive && _activeVehicle.currentMount.mountType===298

    RowLayout{
        id: mainShow

        Item { width: 10; height: 1 }

        ColumnLayout {
            Item { width: 10; height: 1 }
            MMCImageButton {
                //                anchors.horizontalCenter: parent.horizontalCenter
                width: 50
                height: 50
                sourceWidth: 50
                sourceHeight: 50
                checkable: false
                source:    pressed?"qrc:/qmlimages/rotation_press.svg":"qrc:/qmlimages/rotation.svg"
                backColor: "transparent"
                overColor: false
                onPressedChanged:{
                    if(_theMountIsActive) _activeVehicle.currentMount.pitchControl(3, true)
                }
                Timer{
                    interval: 100
                    repeat: true
                    running: parent.longPreseed
                    onTriggered: {
                       if(_theMountIsActive) _activeVehicle.currentMount.pitchControl(3, true)
                    }
                }
            }

            Item { width: 1; Layout.fillHeight: true }

            MMCImageButton {
                //                anchors.horizontalCenter: parent.horizontalCenter
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
                   if(_theMountIsActive) _activeVehicle.currentMount.pitchControl(3, false)
                }
                Timer{
                    interval: 100
                    repeat: true
                    running: parent.longPreseed
                    onTriggered: {
                       if(_theMountIsActive) _activeVehicle.currentMount.pitchControl(3, false)
                    }
                }
            }
            Item { width: 1; height: 10 }
        }

        Item { width: 7; height: 1 }

        Rectangle{
            Layout.fillHeight: true
            width:  2
            color: "#111"
        }
        Item { width: 7; height: 1 }

        Column{
            //                anchors.top:parent.top
            //                anchors.topMargin: 20
            spacing: 10

            Item { width: 7; height: 1 }

            Rectangle{
                id: delayBtn_
                property int step: 0
                property bool hovered: false
                anchors.horizontalCenter: parent.horizontalCenter
                width: 60
                height: 60
                radius: 30
                border.color: delayBtn_.hovered?"#32ae47":"white"
                border.width: 2
                color: "#3e3e3e"
                QGCLabel{
                    anchors.centerIn: parent
                    font.pointSize: 10
                    text: qsTr("Drop", "抛下")
                    color: delayBtn_.hovered?"#32ae47":"#8796B0"
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
                //                    z:                          QGroundControl.zOrderWidgets
                fontPointSize:              _guidedModeBar._fontPointSize
                confirmText:        qsTr("Drop", "抛下")
                textColor: "#8796B0"
                onAccept: {
                    delayBtn_.visible = true
                    dumpConfirm.visible = false
                    if(_theMountIsActive) _activeVehicle.currentMount.dropCmmd()
                }
                onReject: {
                    delayBtn_.visible = true
                    dumpConfirm.visible = false
                }
            }

            QGCLabel{
                visible:  mainShow.visible && _theMountIsActive &&_activeVehicle.currentMount.state === 1
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#00cc00"
                text:  qsTr("The bolt has opened", "抛投已打开")//The bolt has opened
            }

            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 20
                QGCLabel{
                    font.pointSize: 10
                    color:"#8796B0"
                    text: qsTr("Mode:", "智能模式:") //Intelligent Mode:
                }
                MMCSwitch{
                    id: dropSet_
                    checked: _theMountIsActive?_activeVehicle.currentMount.mode===1:false
                    onCheckedChanged:{
                        if(_theMountIsActive){
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

            Column{
                visible: dropSet_.checked
                spacing: 10
                QGCLabel{
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pointSize: 10
                    color:"#8796B0"
                    text: qsTr("Preset throw weight settings(g)", "预设抛投重量(g)") //Preset throw weight settings(g)
                }

                TextField{
                    id: themeEdit
                    anchors.horizontalCenter: parent.horizontalCenter
                    readOnly: false
                    textColor: "black"
                    focus: true;
                    Keys.onReleased: {
                        if(event.key === Qt.Key_Enter || event.key === Qt.Key_Return){
                            acceptWindow(themeStr)
                            themeStr = ""
                            themeEdit.text = ""
                            event.accepted = true;
                            root.close()
                        }
                    }
                }

                Row{
                    anchors.horizontalCenter: parent.horizontalCenter
                    Item{Layout.fillWidth: true; height: 1;}
                    QGCButton{
                        text: qsTr("OK", "确定")
                        width: 60
                        height: 35
                        textColor:"#8796B0"
                        backColor: pressed ? "#333" : "#222"
                        pointSize: pressed ? 10 : 9
                        onDelayClick:{
                           if(_theMountIsActive) _activeVehicle.currentMount.weightControl(parseInt(themeEdit.text))
                        }
                    }
                }

                Item{height: 1; width: 1; }

            }
        }
        Item { width: 10; height: 1 }
    }

}

