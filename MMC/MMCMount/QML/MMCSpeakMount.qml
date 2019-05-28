import QtQuick 2.5
import QtQuick.Layouts 1.2
import QGroundControl.Controls 1.0

Item {
    width:  mainShow.width
    height: mainShow.height
    property bool _theMountIsActive: mountIsActive && _activeVehicle.currentMount.mountType===266
    RowLayout{
        id: mainShow
        Item { width: 10; height: 1 }
        ColumnLayout {
            Item { width: 1; height: 10 }

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
            Item { width: 1; height: 5 }
        }
        Item { width: 7; height: 1 }
        Rectangle{
            Layout.fillHeight: true
            width:  2
            color: "#111"
        }
        Item { width: 7; height: 1 }
        Column{
            spacing: 20
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                QGCLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    font.pointSize: 10
                    color: "#8796B0"
                    text:  qsTr("Volume:", "音量:") //Volume：
                }
                QGCLabel{
                    anchors.verticalCenter: parent.verticalCenter
                    text: _theMountIsActive?_activeVehicle.currentMount.volume:""
                }
            }

            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                Item { height: 1; width: 7; }
                MMCSliderBar{
                    width: 100
                    autoReturnCenter: false
    //                orientation: Qt.Vertical //方向
                    stepSize: 1
                    minimumValue: 200
                    maximumValue: 230
                    enabled: control_volumn.checked
                    value: _theMountIsActive?_activeVehicle.currentMount.volume:200
                    onValueChanged: {
                        if( visible && control_volumn.checked  && _theMountIsActive && value !== _activeVehicle.currentMount.volume && value >= 200)
                            _activeVehicle.currentMount.volumeControl(value)
                    }
                }
                Item { height: 1; width: 10; }
            }

            Row{
                spacing: 10
                anchors.horizontalCenter: parent.horizontalCenter
                QGCLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    font.pointSize: 10
                    color: "#8796B0"
                    text:  qsTr("Enabled", "控制使能:")
                }

                MMCSwitch{
                    anchors.verticalCenter: parent.verticalCenter
                    id: control_volumn
                }
            }

        }
        Item { width: 10; height: 1 }
    }
}
