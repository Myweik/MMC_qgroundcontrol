import QtQuick 2.5
import QtQuick.Layouts 1.2
import QGroundControl.Controls 1.0

Item {
    width:   mainShow.width
    height: mainShow.height
    property bool _theMountIsActive: mountIsActive && _activeVehicle.currentMount.mountType===284
    RowLayout{
        id: mainShow
        property string name: qsTr("Camera", "相机")

        Item { width: 10; height: 1 }

        ColumnLayout {
            Item { width: 1; height: 10 }

            Row {
//                anchors.horizontalCenter: parent.horizontalCenter
                anchors.right: parent.right
                QGCLabel{
                    font.pointSize: 13
                    text: mainShow.name + "1"
                }
                Item{ height:  1; width: 15; }
                Image {
                    source: _theMountIsActive && _activeVehicle.currentMount.cam1 ?
                                "qrc:/qmlimages/resources/customItem/mount/3dmapping/general_state.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/fault_state.svg"
                }
            }

            Row {
//                anchors.horizontalCenter: parent.horizontalCenter
                anchors.right: parent.right
                QGCLabel{
                    font.pointSize: 13
                    text: mainShow.name + "2"
                }
                Item{ height:  1; width: 15; }
                Image {
                    source: _theMountIsActive && _activeVehicle.currentMount.cam2 ?
                                "qrc:/qmlimages/resources/customItem/mount/3dmapping/general_state.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/fault_state.svg"
                }
            }

            Row {
//                anchors.horizontalCenter: parent.horizontalCenter
                anchors.right: parent.right
                QGCLabel{
                    font.pointSize: 13
                    text: mainShow.name + "3"
                }
                Item{ height:  1; width: 15; }
                Image {
                    source: _theMountIsActive && _activeVehicle.currentMount.cam3 ?
                                "qrc:/qmlimages/resources/customItem/mount/3dmapping/general_state.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/fault_state.svg"
                }
            }

            Row {
//                anchors.horizontalCenter: parent.horizontalCenter
                anchors.right: parent.right
                QGCLabel{
                    font.pointSize: 13
                    text: mainShow.name + "4"
                }
                Item{ height:  1; width: 15; }
                Image {
                    source: _theMountIsActive && _activeVehicle.currentMount.cam4 ?
                                "qrc:/qmlimages/resources/customItem/mount/3dmapping/general_state.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/fault_state.svg"
                }
            }

            Row {
//                anchors.horizontalCenter: parent.horizontalCenter
                anchors.right: parent.right
                QGCLabel{
                    font.pointSize: 13
                    text: mainShow.name + "5"
                }
                Item{ height:  1; width: 15; }
                Image {
                    source: _theMountIsActive && _activeVehicle.currentMount.cam5 ?
                                "qrc:/qmlimages/resources/customItem/mount/3dmapping/general_state.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/fault_state.svg"
                }
            }

            Row {
//                anchors.horizontalCenter: parent.horizontalCenter
                anchors.right: parent.right
                QGCLabel{
                    font.pointSize: 13
                    text: qsTr("TF")
                }
                Item{ height:  1; width: 15; }
                Image {
                    source: _theMountIsActive && _activeVehicle.currentMount.tfCard ?
                                "qrc:/qmlimages/resources/customItem/mount/3dmapping/general_state.svg":"qrc:/qmlimages/resources/customItem/mount/3dmapping/fault_state.svg"
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

        Column {
//            anchors.horizontalCenter: parent.horizontalCenter
//            height: 61;
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
                        if(_theMountIsActive){
                            if(_cameraswitch.state == 2) return;
                            if(_cameraswitch.pressed){ //关闭
                                if(_activeVehicle &&  _activeVehicle.currentMount&& _activeVehicle.currentMount.mountType === 284)
                                    _activeVehicle.currentMount.switchControl(false);
                            }else{ //打开
                                if(_activeVehicle &&  _activeVehicle.currentMount&& _activeVehicle.currentMount.mountType === 284)
                                    _activeVehicle.currentMount.switchControl(true);
                            }
                            _cameraswitch.state = 2
                            timer.running = true
                        }
                    }
                }
            }

            Item{ height: 1; width: 1; }

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
                    if(_activeVehicle && !pressed) _activeVehicle.doCameraTrigger() //拍照
                }
            }
        }

        Item { width: 10; height: 1 }
    }

}

