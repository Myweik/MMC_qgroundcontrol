import QtQuick 2.5
import QtQuick.Layouts 1.2
import QGroundControl.Controls 1.0
import QGroundControl 1.0
import QGroundControl.MMCMounts 1.0

Item{
    property alias source: img.source
    property alias type: mountOperation.type
    readonly property var       _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle

    property bool _mainIsImg: true

    width: _mainShow.width
    height: _mainShow.height

    Timer { //防抖
        id: _antiShake
        interval : 100
        repeat : false
        running : false
        triggeredOnStart : false
        onTriggered: {

        }
    }

    MouseArea {
        anchors.fill: parent
//        propagateComposedEvents: true
        onClicked: {
//            console.log("clicked blue")
//            if(_antiShake.running)
//                return
//            _antiShake.start()

//            mouse.accepted = false
        }
//        onPressed:{
//            console.log("Pressed blue")
//            if(_antiShake.running)
//                return
//            _antiShake.start()

//            mouse.accepted = false;
//        }
//        onReleased:{
//            console.log("Released blue");
//            if(_antiShake.running)
//                return
//            _antiShake.start()

//            mouse.accepted = false;
//        }
    }

    function showSwitcher(){
        if(_antiShake.running)
            return
        _antiShake.start()

        _mainIsImg = !_mainIsImg
    }

    Column{
        id: _mainShow

        MMCImageButton {
            id: img
            visible: _mainIsImg
            width: 80
            height: 80
            sourceWidth: 80
            sourceHeight: 80
            checkable: false
            backColor: "transparent"
            overColor: false
            onClicked: {
                showSwitcher()
            }
        }

        Item{
            width: mountOperation.width
            height: mountOperation.height
            visible: !_mainIsImg

            Rectangle{ //背景
                anchors.fill: parent
                opacity: 0.85
                color: "#3e3e3e"
            }

            MMCMountOperation {
                id: mountOperation
                z: parent.z +1
            }
        }
    }
}

