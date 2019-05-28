import QtQuick 2.5
import QtQuick.Layouts 1.2
import QGroundControl.Controls 1.0

Item {
    width:   mainShow.width
    height: mainShow.height
    property bool _theMountIsActive: mountIsActive && _activeVehicle.currentMount.mountType===346
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
            id: column_gas
            spacing: 10
            property var gasList: _theMountIsActive?_activeVehicle.currentMount.gasList: ""
            Repeater{
                model: column_gas.gasList
                delegate: Row{
                    spacing: 5
                    QGCLabel {
                        font.pointSize: 10
                        color: "#8796B0"
                        text:  column_gas.gasList.get(index).name + ":"
                    }
                    QGCLabel {
                        font.pointSize: 10
                        color: "#8796B0"
                        text:  column_gas.gasList.get(index).concentration.toFixed(2)
                    }
                    QGCLabel {
                        font.pointSize: 10
                        color: "#8796B0"
                        text:  "("+column_gas.gasList.get(index).uintName+")"
                    }
                }

            }
        }
        Item { width: 10; height: 1 }
      }
}
