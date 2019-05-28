import QtQuick 2.5
import QtQuick.Layouts 1.2
import QGroundControl.Controls 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Item {
    width: mainShow.width
    height: mainShow.height
    property bool _theMountIsActive: mountIsActive && _activeVehicle.currentMount.mountType===0
    ColumnLayout {
        id: mainShow
        width: 150
        spacing: 2
        Column{
            Layout.fillWidth: true
            MMCSliderBar{
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 10
                height: 30
                minimumValue: 0
                maximumValue: 2
                autoReturnCenter: false
                value: _theMountIsActive ?_activeVehicle.currentMount.mode:0
                stepSize: 1
                isSwidth: true
                onValueChanged:{
                    if(_theMountIsActive && value>=0 && value !== _activeVehicle.currentMount.mode)
                        _activeVehicle.currentMount.controlMode(value)
                }
            }
            QGCLabel{
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 10
                color:"#8796B0"
                text: qsTr("Lock  Follow  Center", "锁定     跟随     回中")
            }
        }

        Rectangle{
            Layout.fillWidth: true
            visible:  _theMountIsActive &&
                      (_activeVehicle.currentMount.cam_type === 491  || _activeVehicle.currentMount.cam_type ===  443 || _activeVehicle.currentMount.cam_type ===  523)
            height: 2
            color: "#111"
        }

        Column{  //Filr红外色板切换
            Layout.fillWidth: true
            visible: _theMountIsActive && (_activeVehicle.currentMount.cam_type === 443 || _activeVehicle.currentMount.cam_type === 523)
            MMCSliderBar{
                id: _colorPlate
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 10
                minimumValue: 0
                maximumValue: 2
                autoReturnCenter: false
                stepSize: 1
                isSwidth: true
                value: _theMountIsActive && (_activeVehicle.currentMount.cam_type === 443 || _activeVehicle.currentMount.cam_type === 523) ?_activeVehicle.currentMount.colorPlate:0
                onValueChanged:{
                    if(visible && _theMountIsActive && value>=0 && value !== _activeVehicle.currentMount.colorPlate)
                        _activeVehicle.currentMount.controlColorPlate(value)
                }
            }
            QGCLabel{
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 10
                color:"#8796B0"
                text: qsTr("BW    WB    RY", "黑白    白黑    红黄") //BW     WB     RY
            }
        }

        Column{  //高清双光显示板
            Layout.fillWidth: true
            visible: _theMountIsActive && _activeVehicle.currentMount.cam_type === 523
            width: 80
            MMCSliderBar{
                id: _frame
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 10
                minimumValue: 0
                maximumValue: 1
                autoReturnCenter: false
                stepSize: 1
                isSwidth: true
                value: _theMountIsActive && _activeVehicle.currentMount.cam_type === 523 ?_activeVehicle.currentMount.frame:0
                onValueChanged:{
                    if(visible && _theMountIsActive && value>=0 && value !== _activeVehicle.currentMount.frame)
                        _activeVehicle.currentMount.controlFrame(value)
                }
            }
            QGCLabel{
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 10
                color:"#8796B0"
                text: qsTr("HD    IR", "高清    红外") //HD      IR
            }
        }

        MMCComboBox { //海视英科红外色板切换
            id: _directionsCombox
            visible: _theMountIsActive && _activeVehicle.currentMount.cam_type === 491
            Layout.fillWidth: true
            height: 25
            pointSize: 10
            textColor: "#8796B0"
            fontColor: textColor
            backColor: "transparent"
            fontSize: 13
            text: _theMountIsActive && _activeVehicle.currentMount.cam_type === 491 ? _hsykItems.get(_activeVehicle.currentMount.colorPlate).text : ""
            model: ListModel {
                id: _hsykItems
                ListElement { text: qsTr("WhiteModel", "白热模式");} //WhiteModel
                ListElement { text: qsTr("RedModel", "红热模式");} //RedModel
                ListElement { text: qsTr("AmberMolor", "琥珀色");}  //AmberMolor
                ListElement { text: qsTr("YellowModel", "黄热模式");} //YellowModel
            }
            onCurrentIndexChanged:{
                var value = currentIndex
                if( visible && _theMountIsActive && value>=0 && value !== _activeVehicle.currentMount.colorPlate)
                    _activeVehicle.currentMount.controlColorPlate(value)
            }
        }

        Rectangle{
            Layout.fillWidth: true
            height: 2
            color: "#111"
        }

        /* 变焦 */
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            Item{height: 1; width: 5;}

            QGCLabel{
                //                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize:10
                color:"#8796B0"
                text: qsTr("Zoom", "变焦")
            }
            Item{height: 1; width: 5;}
            MMCImageButton {
                id: zoom_up
                width: 50
                height: 50
                sourceWidth: 50
                sourceHeight: 50
                checkable: false
                enabled:  mainShow.visible && _theMountIsActive && _activeVehicle.currentMount.zoomUseState === 1
                source:    mainShow.visible && _theMountIsActive && _activeVehicle.currentMount.zoomUseState === 2 ? "/qmlimages/plus_click.svg" : !pressed?"/qmlimages/plus.svg":"/qmlimages/plus_click.svg"
                backColor: "transparent"
                overColor: false
                visibleChangedEnabled: true
                onPressedChanged:{
                    if(_theMountIsActive){
                        if(pressed) _activeVehicle.currentMount.controlZoom(2)
                        else  _activeVehicle.currentMount.controlZoom(1)
                    }
                }
                Timer{
                    interval: 100
                    repeat: true
                    running: zoom_up.pressed
                    onTriggered: {
                       if(_theMountIsActive)  _activeVehicle.currentMount.controlZoom(2)
                    }
                }
            }
            Item{height: 1; width: 5;}
            MMCImageButton {
                id: zoom_down
                width: 50
                height: 50
                sourceWidth: 50
                sourceHeight: 50
                checkable: false
                enabled:   mainShow.visible && _theMountIsActive && _activeVehicle.currentMount.zoomUseState === 1
                source:   mainShow.visible && _theMountIsActive && _activeVehicle.currentMount.zoomUseState === 0 ? "/qmlimages/reduce_click.svg" : !pressed?"/qmlimages/reduce.svg":"/qmlimages/reduce_click.svg"
                backColor: "transparent"
                overColor: false
                visibleChangedEnabled: true
                onPressedChanged:{
                    if(_theMountIsActive){
                        if(pressed) _activeVehicle.currentMount.controlZoom(0)
                        else  _activeVehicle.currentMount.controlZoom(1)
                    }
                }
                Timer{
                    interval: 100
                    repeat: true
                    running: zoom_down.pressed
                    onTriggered: {
                        if(_theMountIsActive) _activeVehicle.currentMount.controlZoom(0)
                    }
                }
            }

            Item{height: 1; width: 5;}
        }

        Rectangle{
            Layout.fillWidth: true
            height: 2
            color: "#111"
        }

        /* 聚焦 */
        Row {
            visible: mainShow.visible && _theMountIsActive && _activeVehicle.currentMount.cam_type === 283
            anchors.horizontalCenter: parent.horizontalCenter
            Item{height: 1; width: 5;}

            QGCLabel{
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize:10
                color:"#8796B0"
                text: qsTr("Focus", "聚焦")
            }
            Item{height: 1; width: 5;}
            MMCImageButton {
                id: focus_up
                width: 50
                height: 50
                sourceWidth: 50
                sourceHeight: 50
                checkable: false
                enabled:  true
                source:    !pressed?"/qmlimages/plus.svg":"/qmlimages/plus_click.svg"
                backColor: "transparent"
                overColor: false
                visibleChangedEnabled: true
                onPressedChanged:{
                }
                Timer{
                    interval: 500
                    repeat: true
                    triggeredOnStart: true
                    running: focus_up.pressed
                    onTriggered: {
                       if(_theMountIsActive)  _activeVehicle.currentMount.controlCommand("55 aa 25 04 00 00 01 00 00 2a")
                    }
                }
            }
            Item{height: 1; width: 5;}
            MMCImageButton {
                id: focus_down
                width: 50
                height: 50
                sourceWidth: 50
                sourceHeight: 50
                checkable: false
                enabled:   true
                source:   !pressed?"/qmlimages/reduce.svg":"/qmlimages/reduce_click.svg"
                backColor: "transparent"
                overColor: false
                visibleChangedEnabled: true
                onPressedChanged:{

                }
                Timer{
                    interval: 500
                    repeat: true
                    triggeredOnStart: true
                    running: focus_down.pressed
                    onTriggered: {
                        if(_theMountIsActive) _activeVehicle.currentMount.controlCommand("55 aa 25 04 00 00 02 00 00 2b")
                    }
                }
            }

            Item{height: 1; width: 5;}
        }

        Rectangle{
            visible: mainShow.visible && _theMountIsActive && _activeVehicle.currentMount.cam_type === 283
            Layout.fillWidth: true
            height: 2
            color: "#111"
        }

        Grid{
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalItemAlignment: Grid.AlignHCenter
            verticalItemAlignment: Grid.AlignVCenter
            columns: 1
            columnSpacing: 2
            rowSpacing: 2
            enabled:  _theMountIsActive && _activeVehicle.currentMount.mountType===0

            JoystickThumbPad {
                id: joystic_
                width: 100
                height: 100
                onXAxisChanged:{
                    if(pitchTimer_.running)
                        return;
                    var tmpValue = (xAxis*5).toFixed(0);
                    if(tmpValue > 4)
                    {
                        headTimer_.start()
                        _activeVehicle.currentMount.controlHead(3, true)
                    }
                    else if(tmpValue < -4)
                    {
                        headTimer_.start()
                        _activeVehicle.currentMount.controlHead(3, false)
                    }
                    else
                    {
                        headTimer_.stop()
                        _activeVehicle.currentMount.controlHead(0, false)
                    }
                }
                onYAxisChanged:{
                    if(headTimer_.running)
                        return;
                    var tmpValue = (yAxis*5).toFixed(0);
                    if(tmpValue > 4)
                    {
                        _activeVehicle.currentMount.controlPitch(3, true)
                        pitchTimer_.start()
                    }
                    else if(tmpValue < -4)
                    {
                        pitchTimer_.start()
                        _activeVehicle.currentMount.controlPitch(3, false)
                    }
                    else
                    {
                        pitchTimer_.stop()
                        _activeVehicle.currentMount.controlPitch(0, false)
                    }
                }

                Timer{
                    id: pitchTimer_
                    interval: 100
                    repeat: true
                    onTriggered: {
                        var tmpValue = (joystic_.yAxis*5).toFixed(0);
                        if(tmpValue > 4)
                            _activeVehicle.currentMount.controlPitch(3, true)
                        else if(tmpValue < -4)
                            _activeVehicle.currentMount.controlPitch(3, false)
                    }
                }
                Timer{
                    id: headTimer_
                    interval: 100
                    repeat: true
                    onTriggered: {
                        var tmpValue = (joystic_.xAxis*5).toFixed(0);
                        if(tmpValue > 4)
                            _activeVehicle.currentMount.controlHead(3, true)
                        else if(tmpValue < -4)
                            _activeVehicle.currentMount.controlHead(3, false)
                    }
                }
            }
        }

        Rectangle{
            Layout.fillWidth: true
            height: 2
            color: "#111"
        }

        Row {
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter
            Column{
                MMCImageButton {
                    width: 50
                    height: 50
                    sourceWidth: 50
                    sourceHeight: 50
                    checkable: false
                    source:    mainShow.visible && _activeVehicle && _activeVehicle.cameraTriggerUseClick === 2 ? "/qmlimages/photo_click.svg" : !pressed?"/qmlimages/photo.svg":"/qmlimages/photo_click.svg"
                    backColor: "transparent"
                    overColor: false
                    onClicked: {
                       if(_activeVehicle) _activeVehicle.doCameraTrigger()
                    }
                }
            }

            Column{
                MMCImageButton {
                    width: 50
                    height: 50
                    sourceWidth: 50
                    sourceHeight: 50
                    checkable: false
                    source: {
                        if(_theMountIsActive){
                        return _activeVehicle.currentMount.videoUseClick === 2 ? "qrc:/qmlimages/shoot_click.svg" :_activeVehicle.currentMount.videoStatus === 1? pressed? "qrc:/qmlimages/shoot_click.svg" :"qrc:/qmlimages/shoot_ing.svg":
                        pressed?"qrc:/qmlimages/shoot_click.svg" : "qrc:/qmlimages/shoot.svg"
                        }else return pressed? "qrc:/qmlimages/shoot_click.svg" :"qrc:/qmlimages/shoot.svg"
                    }
                    backColor: "transparent"
                    overColor: false
                    onClicked: {
                        if(_theMountIsActive) _activeVehicle.currentMount.videoTape()
                    }
                }
            }
        }

        Item { width: 1; height: 3 }
    }
}


