import QtQuick 2.5
import QtQuick.Layouts 1.2
import QGroundControl.Controls 1.0

Item {
    width:   mainShow.width
    height: mainShow.height
    property bool _theMountIsActive: mountIsActive && _activeVehicle.currentMount.mountType===282
    RowLayout{
        id: mainShow
        Item { width: 10; height: 1 }
        ColumnLayout {
            Item { width: 1; height: 10 }
            MMCImageButton {
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
        MMCLightSwitch{
            srcOn: _theMountIsActive ? _activeVehicle.currentMount.state===1 :  false
            onOnChanged:{
                if(_theMountIsActive){
                    if(_activeVehicle.currentMount.state < 0)
                        return;
                    if(on){
                        if(_activeVehicle.currentMount.state!==1)
                        _activeVehicle.currentMount.lightControl(true)
                    }else{
                        if(_activeVehicle.currentMount.state!==0)
                        _activeVehicle.currentMount.lightControl(false)
                    }
                }
            }
        }
        Item { width: 10; height: 1 }
      }
}
