import QtQuick 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0

Item {
    width:row_.width + 20
    height: row_.height + 20
    Row {
        id: row_
        anchors.centerIn: parent
        spacing: 10
        Rectangle{
            height: 310
            width: 320
            color: "#282828"
            Rectangle{
                anchors.fill: parent
                anchors.margins: 1
                border.color: "#4e4d4d"
                color: "#3e3e3e"
                Column {
                    id: _cameraState
                    anchors.left:  parent.left
                    anchors.top:  parent.top
                    anchors.right: parent.right
                    anchors.leftMargin: 15
                    anchors.rightMargin: 15
                    anchors.topMargin: 20
                    property string name: qsTr("Camera", "相机")

                    Row {
                        spacing: 10
                        QGCLabel {
                            color: "#aaa"
                            font.pointSize: 13
                            text: qsTr(" WifiName:", "WIFI账号:")
                        }
                        QGCLabel {
                            font.pointSize: 13
                            text: mountIsActive&& _activeVehicle.currentMount.mountType === 284 && _activeVehicle.currentMount.wifiName ? "DIRECT-" + _activeVehicle.currentMount.wifiName + ":DSC-RX0":""
                        }
                    }

                    Item{ height: 16; width: 1; }

                    Row {
                        spacing: 10
                        QGCLabel {
                            color: "#aaa"
                            font.pointSize: 13
                            text: qsTr("PassWord:", "WIFI密码:")
                        }
                        QGCLabel{
                            font.pointSize: 13
                            text: mountIsActive && _activeVehicle.currentMount.mountType === 284 && _activeVehicle.currentMount.wifiPassword ? _activeVehicle.currentMount.wifiPassword :""
                        }
                    }

                    Item{ height: 16; width: 1; }

                    Item{
                        height: 20
                        width: parent.width
                        Row {
                            anchors.left:  parent.left
                            QGCLabel{
                                font.pointSize: 13
                                text: _cameraState.name + "1"
                            }
                            Item{ height:  1; width: 15; }
                            Image {
                                source: mountIsActive&& _activeVehicle.currentMount.mountType === 284 && _activeVehicle.currentMount.cam1 ?
                                            "qrc:/qmlimages/resources/customItem/mount/3dmapping/general_state.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/fault_state.svg"
                            }
                        }

                        Row {
                            anchors.horizontalCenter: parent.horizontalCenter
                            // anchors.verticalCenter: parent.verticalCenter
                            //anchors
                            QGCLabel{
                                font.pointSize: 13
                                text: _cameraState.name + "2"
                            }
                            Item{ height:  1; width: 15; }
                            Image {
                                source: mountIsActive && _activeVehicle.currentMount.mountType === 284 && _activeVehicle.currentMount.cam2 ?
                                            "qrc:/qmlimages/resources/customItem/mount/3dmapping/general_state.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/fault_state.svg"
                            }
                        }

                        Row {
                            anchors.right: parent.right
                            QGCLabel{
                                font.pointSize: 13
                                text: _cameraState.name + "3"
                            }
                            Item{ height:  1; width: 15; }
                            Image {
                                source: mountIsActive&& _activeVehicle.currentMount.mountType === 284 && _activeVehicle.currentMount.cam3 ?
                                            "qrc:/qmlimages/resources/customItem/mount/3dmapping/general_state.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/fault_state.svg"
                            }
                        }
                    }

                    Item{ height: 28; width: 1; }

                    Item{
                        height: 20
                        width: parent.width
                        Row {
                            anchors.left:  parent.left
                            QGCLabel{
                                font.pointSize: 13
                                text: _cameraState.name + "4"
                            }
                            Item{ height:  1; width: 15; }
                            Image {
                                source: mountIsActive && _activeVehicle.currentMount.mountType === 284 && _activeVehicle.currentMount.cam4 ?
                                            "qrc:/qmlimages/resources/customItem/mount/3dmapping/general_state.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/fault_state.svg"
                            }
                        }

                        Row {
                            //anchors.left:  parent.left
                            anchors.right: parent.right
                            // anchors.verticalCenter: parent.verticalCenter
                            QGCLabel{
                                font.pointSize: 13
                                text: _cameraState.name + "5"
                            }
                            Item{ height:  1; width: 15; }
                            Image {
                                source: mountIsActive && _activeVehicle.currentMount.mountType === 284 && _activeVehicle.currentMount.cam5 ?
                                            "qrc:/qmlimages/resources/customItem/mount/3dmapping/general_state.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/fault_state.svg"
                            }
                        }

                        Row {
                            anchors.horizontalCenter: parent.horizontalCenter
                            QGCLabel{
                                font.pointSize: 13
                                text: qsTr("TF")
                            }
                            Item{ height:  1; width: 15; }
                            Image {
                                source: mountIsActive && _activeVehicle.currentMount.mountType === 284 && _activeVehicle.currentMount.tfCard ?
                                            "qrc:/qmlimages/resources/customItem/mount/3dmapping/general_state.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/fault_state.svg"
                            }
                        }
                    }

                    Item{ height: 30; width: 1; }

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: 61;
                        Item {
                            id: _cameraswitch
                            width: 65
                            height: 65
                            property bool pressed : false
                            property int state: 1  //1 - 正常状态  2 - 命令下发状态
                            Canvas {
                                visible: _cameraswitch.state === 2
                                id: canvas
                                anchors.fill: parent
                                antialiasing: true
                                property color primaryColor: "#313131" //_cameraswitch.pressed ? "#313131"   //"orange"
                                property color secondaryColor: "#fff" //"lightblue"// _cameraswitch.pressed ?  //"lightblue"
                                property real centerWidth: width / 2
                                property real centerHeight: height / 2
                                //property real radius: Math.min(canvas.width, canvas.height) / 2
                                property real radius: Math.min(canvas.width-10, canvas.height-10) / 2

                                property real minimumValue: 0
                                property real maximumValue: 100
                                property real currentValue: 0

                                // this is the angle that splits the circle in two arcs
                                // first arc is drawn from 0 radians to angle radians
                                // second arc is angle radians to 2*PI radians
                                property real angle: (currentValue - minimumValue) / (maximumValue - minimumValue) * 2 * Math.PI

                                // we want both circle to start / end at 12 o'clock
                                // without this offset we would start / end at 9 o'clock
                                property real angleOffset: -Math.PI / 2

                                onPrimaryColorChanged: requestPaint()
                                onSecondaryColorChanged: requestPaint()
                                onMinimumValueChanged: requestPaint()
                                onMaximumValueChanged: requestPaint()
                                onCurrentValueChanged: requestPaint()

                                onPaint: {
                                    var ctx = getContext("2d");
                                    ctx.save();
                                    ctx.clearRect(0, 0, canvas.width, canvas.height);
                                    ctx.beginPath();
                                    ctx.lineWidth = 10;
                                    ctx.strokeStyle = primaryColor;
                                    ctx.arc(canvas.centerWidth,
                                            canvas.centerHeight,
                                            canvas.radius,
                                            angleOffset + canvas.angle,
                                            angleOffset + 2*Math.PI);
                                    ctx.stroke();

                                    ctx.beginPath();
                                    ctx.lineWidth = 10;
                                    ctx.strokeStyle = canvas.secondaryColor;
                                    ctx.arc(canvas.centerWidth,
                                            canvas.centerHeight,
                                            canvas.radius,
                                            canvas.angleOffset,
                                            canvas.angleOffset + canvas.angle);
                                    ctx.stroke();
                                    ctx.restore();
                                }

                                Timer{
                                    id: timer
                                    interval: 200;
                                    running: false;
                                    repeat: true;
                                    onTriggered: {
                                        if(canvas.currentValue === 100) {
                                            canvas.currentValue = 0
                                            _cameraswitch.state = 1
                                            _cameraswitch.pressed = !_cameraswitch.pressed
                                            timer.running = false
                                        }
                                        canvas.currentValue += 1;
                                    }
                                }
                            }

                            MMCImageButton { //开关
                                anchors.centerIn: parent
                                width: 65
                                height: 65
                                sourceWidth: 65
                                sourceHeight: 65
                                checkable: false
                                source:    _cameraswitch.pressed?"qrc:/qmlimages/resources/customItem/mount/3dmapping/switch_click.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/switch.svg"
                                backColor: "transparent"
                                overColor: false
                                onPressedChanged:{
                                    if((mountIsActive && _activeVehicle.currentMount.mountType === 284)){
                                        if( _cameraswitch.state == 2) return;
                                    if(_cameraswitch.pressed){ //关闭
                                        _activeVehicle.currentMount.switchControl(false);
                                    }else{ //打开
                                        _activeVehicle.currentMount.switchControl(true);
                                    }
                                    _cameraswitch.state = 2
                                    timer.running = true
                                    }
                                }
                            }
                        }

                        Item{ height: 1; width: 58; }

                        MMCImageButton { //拍照
                            // anchors.centerIn: parent
                            width: 61
                            height: 61
                            sourceWidth: 61
                            sourceHeight: 61
                            checkable: false
                            source:    pressed?"qrc:/qmlimages/resources/customItem/mount/3dmapping/Photograph_click.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/Photograph.svg"
                            backColor: "transparent"
                            overColor: false
                            onPressedChanged:{
                                if(!pressed) _activeVehicle.doCameraTrigger() //拍照
                            }
                        }
                    }

                    Item{ height: 28; width: 1; }

                    Row {
                        spacing: 85
                        QGCLabel{
                            font.pointSize: 13
                            text: mountIsActive&& _activeVehicle.currentMount.mountType === 284?_activeVehicle.currentMount.fileName :""
                        }

                        QGCLabel{
                            font.pointSize: 13
                            text: mountIsActive&& _activeVehicle.currentMount.mountType === 284? "Number" + _activeVehicle.currentMount.photoNumber :""
                        }
                    }
                }
            }
        }

        Column{
            anchors.verticalCenter: parent.verticalCenter
            Image{
                source: "/qmlimages/camera3dmap.png"
            }

            QGCLabel{
                anchors.horizontalCenter: parent.horizontalCenter
                text: {
                    if(mountIsActive)
                    {
                        if(_activeVehicle.currentMount.mountType === 0)
                            return _activeVehicle.currentMount.mountName(_activeVehicle.currentMount.cam_type)
                        else
                            return _activeVehicle.currentMount.mountName(_activeVehicle.currentMount.mountType)
                    }
                    return ""
                }
            }
            QGCLabel{
                anchors.horizontalCenter: parent.horizontalCenter
                text: mountIsActive? _activeVehicle.currentMount.version:""
            }
        }
        Component.onCompleted: {
            isCompleted = true;
        }
    }

}

