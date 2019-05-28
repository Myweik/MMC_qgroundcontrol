import QtQuick 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0

Item {
    width:column_.width + 20
    height: column_.height + 20
//    property bool isCompleted: false
    Column{
        id: column_
        anchors.centerIn: parent
        spacing: 5
        //信息显示位置
        Row{
            spacing: 20
            Row{
                QGCLabel {
                    font.pointSize: 10
                    text: qsTr("Pitch：", "俯仰:")
                }
                QGCLabel {
                    font.pointSize: 10
                    text:  mountIsActive?_activeVehicle.currentMount.pitchAngle.toFixed(4):""
                }
            }
            Row{
                QGCLabel {
                    font.pointSize: 10
                    text:  qsTr("Head:", "偏航:")
                }
                QGCLabel {
                    font.pointSize: 10
                    text:  mountIsActive?_activeVehicle.currentMount.headAngle.toFixed(4):""
                }
            }
            Row{
                QGCLabel {
                    font.pointSize: 10
                    text:  qsTr("Roll：", "横滚:")
                }
                QGCLabel {
                    font.pointSize: 10
                    text:  mountIsActive?_activeVehicle.currentMount.rollAngle.toFixed(4):""
                }
            }
            Row{
                QGCLabel {
                    font.pointSize: 10
                    text:  qsTr("Zoom:", "变焦:")
                }
                QGCLabel {
                    font.pointSize: 10
                    text:  mountIsActive?_activeVehicle.currentMount.zoom.toFixed(0):""
                }
            }
            Row{
                visible: mountIsActive && _activeVehicle.currentMount.zoomPosition.toFixed(0) != 0
                QGCLabel {
                    font.pointSize: 10
                    text:  qsTr("Position:", "位置:")
                }
                QGCLabel {
                    font.pointSize: 10
                    text:  mountIsActive?_activeVehicle.currentMount.zoomPosition.toFixed(0):""
                }
            }

            Row{
                QGCLabel {
                    font.pointSize: 10
                    text:  qsTr("PhotoIndex:", "照片反馈:")
                }
                QGCLabel {
                    font.pointSize: 10
                    text:  mountIsActive?_activeVehicle.currentMount.tackPictIndex.toFixed(0):""
                }
            }
        }
        //变焦相机
        Row{
            spacing: 20
            Rectangle{
                height: 290
                width: 200
                color: "#282828"
                Rectangle{
                    anchors.fill: parent
                    anchors.margins: 1
                    border.color: "#4e4d4d"
                    color: "#3e3e3e"
                    Column{
                        anchors.centerIn: parent
                        spacing: 20
                        Grid{
                            horizontalItemAlignment: Grid.AlignHCenter
                            verticalItemAlignment: Grid.AlignVCenter
                            columns: 3
                            columnSpacing: 10
                            rowSpacing: 10
                            enabled:  mountIsActive && _activeVehicle.currentMount.mountType===0
                            Item{
                                width: 1
                                height: 1
                            }
                            MMCImageButton {
                                id: pitch_up
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
                                    {
                                        if(pressed)
                                            _activeVehicle.currentMount.controlPitch(1*(speed_control.currentIndex+1), true)
                                        else
                                            _activeVehicle.currentMount.controlPitch(0, true)
                                    }
//                                    console.log("1*(speed_control.currentIndex+1) \n",1*(speed_control.currentIndex+1))
                                }
                                Timer{
                                    interval: 100
                                    repeat: true
                                    running: pitch_up.longPreseed
                                    onTriggered: {
                                        if(mountIsActive)
                                            _activeVehicle.currentMount.controlPitch(1*(speed_control.currentIndex+1), true)
//                                        console.log("1*(speed_control.currentIndex+1) \n",1*(speed_control.currentIndex+1))
                                    }
                                }
                            }
                            Item{
                                width: 1
                                height: 1
                            }
                            MMCImageButton {
                                id: head_left
                                width: 50
                                height: 50
                                sourceWidth: 50
                                sourceHeight: 50
                                checkable: false
                                rotation: -90
                                source:    pressed?"qrc:/qmlimages/rotation_press.svg":"qrc:/qmlimages/rotation.svg"
                                backColor: "transparent"
                                overColor: false
                                onPressedChanged:{
                                    if(mountIsActive)
                                    {
                                        if(pressed)
                                            _activeVehicle.currentMount.controlHead(1*(speed_control.currentIndex+1), false)
                                        else
                                            _activeVehicle.currentMount.controlHead(0, false)
                                    }
                                }
                                Timer{
                                    interval: 100
                                    repeat: true
                                    running: head_left.longPreseed
                                    onTriggered: {
                                        if(mountIsActive)
                                            _activeVehicle.currentMount.controlHead(1*(speed_control.currentIndex+1), false)
                                    }
                                }
                            }
                            JoystickThumbPad {
                                id: joystic_
                                width: 60
                                height: 60
                                onXAxisChanged:{
                                    if(pitchTimer_.running)
                                        return;
                                    var tmpValue = (xAxis*5).toFixed(0);
                                    if(mountIsActive)
                                    {
                                        if(tmpValue > 4)
                                        {
                                            headTimer_.start()
                                            _activeVehicle.currentMount.controlHead((speed_control.currentIndex+1), true)
                                        }
                                        else if(tmpValue < -4)
                                        {
                                            headTimer_.start()
                                            _activeVehicle.currentMount.controlHead((speed_control.currentIndex+1), false)
                                        }
                                        else
                                        {
                                            headTimer_.stop()
                                            _activeVehicle.currentMount.controlHead(0, false)
                                        }
                                    }
                                }
                                onYAxisChanged:{
                                    if(headTimer_.running)
                                        return;
                                    var tmpValue = (yAxis*5).toFixed(0);
                                    if(mountIsActive)
                                    {
                                        if(tmpValue > 4)
                                        {
                                            _activeVehicle.currentMount.controlPitch((speed_control.currentIndex+1), true)
                                            pitchTimer_.start()
                                        }
                                        else if(tmpValue < -4)
                                        {
                                            pitchTimer_.start()
                                            _activeVehicle.currentMount.controlPitch((speed_control.currentIndex+1), false)
                                        }
                                        else
                                        {
                                            pitchTimer_.stop()
                                            _activeVehicle.currentMount.controlPitch(0, false)
                                        }
                                    }
                                }

                                Timer{
                                    id: pitchTimer_
                                    interval: 100
                                    repeat: true
                                    onTriggered: {
                                        if(mountIsActive){
                                            //&& win_.visibility
                                            var tmpValue = (joystic_.yAxis*5).toFixed(0);
                                            if(tmpValue > 4)
                                                _activeVehicle.currentMount.controlPitch((speed_control.currentIndex+1), true)
                                            else if(tmpValue < -4)
                                                _activeVehicle.currentMount.controlPitch((speed_control.currentIndex+1), false)
                                        }
                                    }
                                }
                                Timer{
                                    id: headTimer_
                                    interval: 100
                                    repeat: true
                                    onTriggered: {
                                        if(mountIsActive){
                                            var tmpValue = (joystic_.xAxis*5).toFixed(0);
                                            if(tmpValue > 4)
                                                _activeVehicle.currentMount.controlHead((speed_control.currentIndex+1), true)
                                            else if(tmpValue < -4)
                                                _activeVehicle.currentMount.controlHead((speed_control.currentIndex+1), false)
                                        }
                                    }
                                }
                            }
                            MMCImageButton {
                                id: head_right
                                width: 50
                                height: 50
                                sourceWidth: 50
                                sourceHeight: 50
                                checkable: false
                                rotation: 90
                                source:    pressed?"qrc:/qmlimages/rotation_press.svg":"qrc:/qmlimages/rotation.svg"
                                backColor: "transparent"
                                overColor: false
                                onPressedChanged:{
                                    if(mountIsActive)
                                    {
                                        if(pressed)
                                            _activeVehicle.currentMount.controlHead(1*(speed_control.currentIndex+1), true)
                                        else
                                            _activeVehicle.currentMount.controlHead(0, true)
                                    }
                                }
                                Timer{
                                    interval: 100
                                    repeat: true
                                    running: head_right.longPreseed
                                    onTriggered: {
                                        if(mountIsActive)
                                            _activeVehicle.currentMount.controlHead(1*(speed_control.currentIndex+1), true)
                                    }
                                }
                            }
                            Item{
                                width: 1
                                height: 1
                            }
                            MMCImageButton {
                                id: pitch_down
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
                                    {
                                        if(pressed)
                                            _activeVehicle.currentMount.controlPitch(1*(speed_control.currentIndex+1), false)
                                        else
                                            _activeVehicle.currentMount.controlPitch(0, false)
                                    }
                                }
                                Timer{
                                    interval: 100
                                    repeat: true
                                    running: pitch_down.longPreseed
                                    onTriggered: {
                                        if(mountIsActive) _activeVehicle.currentMount.controlPitch(1*(speed_control.currentIndex+1), false)
                                    }
                                }
                            }
                            Item{
                                width: 1
                                height: 1
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
                                id: speed_control
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
                width: 230
                color: "#282828"
                Rectangle{
                    anchors.fill: parent
                    anchors.margins: 1
                    border.color: "#4e4d4d"
                    color: "#3e3e3e"
                    Column{
                        anchors.centerIn: parent
                        spacing: 10
                        Grid{
                            anchors.horizontalCenter: parent.horizontalCenter
                            columns: 2
                            columnSpacing: 5
                            Column{
                                MMCSliderBar{
                                    id:infraredMode
                                    width: parent.width
                                    minimumValue: 0
                                    maximumValue: 2
                                    autoReturnCenter: false
                                    value: mountIsActive && _activeVehicle.currentMount.mountType===0?_activeVehicle.currentMount.mode:0
                                    stepSize: 1
                                    isSwidth: true
                                    onValueChanged:{
                                        if(mountIsActive && _activeVehicle.currentMount.mountType===0
                                                && isCompleted && value >= 0 && value !== _activeVehicle.currentMount.mode)
                                            _activeVehicle.currentMount.controlMode(value)
                                    }
                                }

                                QGCLabel{
                                    font.pointSize:8
                                    text: qsTr("Lock  Free  Center", "锁定 跟随 回中")
                                }
                            }

                            Item { //海视英科红外色板切换
                                width: 90
                                height: 20
                                visible: mountIsActive && _activeVehicle.currentMount.mountType===0 && _activeVehicle.currentMount.cam_type === 491
                                y: 6
                                Rectangle{
                                    z: 10
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: parent.width - 20
                                    height: parent.height - 4
                                    color: "#666"
                                    Text {
                                        anchors.left: parent.left
                                        anchors.leftMargin: 10
                                        font.pixelSize: 13
                                        font.family: "Arial"
                                        color: "#fff"
                                        text: mountIsActive &&  _activeVehicle.currentMount.mountType===0 && _activeVehicle.currentMount.cam_type === 491 ? _hsykItems.get(_activeVehicle.currentMount.colorPlate).text : ""
                                    }
                                }

                                ComboBox {  //src
                                    width: parent.width
                                    height: parent.height
                                    model: ListModel {
                                        id: _hsykItems
                                        ListElement { text: qsTr("WhiteModel", "白热模式");}
                                        ListElement { text: qsTr("RedModel", "红热模式");}
                                        ListElement { text: qsTr("AmberMolor", "琥珀色");}
                                        ListElement { text: qsTr("YellowModel", "黄热模式");}
                                    }
                                    onActivated: {
                                        var value = index
                                        if(mountIsActive && isCompleted && _activeVehicle.currentMount.mountType===0 && _activeVehicle.currentMount.cam_type === 491 && value>=0 && value !== _activeVehicle.currentMount.colorPlate)
                                            _activeVehicle.currentMount.controlColorPlate(value)
                                    }
                                    style: ComboBoxStyle{
                                        font.pixelSize: 13
                                        background: Item {
                                            Rectangle {
                                                width:  parent.width+2
                                                height: parent.height
                                                color:  "#666"
                                                Image {
                                                    source: "/qmlimages/arrow-down.png"
                                                    anchors.verticalCenter: parent.verticalCenter
                                                    anchors.right: parent.right
                                                    anchors.rightMargin: dropDownButtonWidth / 2
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            Column{  //高清双光显示板
                                visible: mountIsActive && _activeVehicle.currentMount.mountType===0 && _activeVehicle.currentMount.cam_type === 523
                                width: 80
                                MMCSliderBar{
                                    id: _frame
                                    width: parent.width
                                    minimumValue: 0
                                    maximumValue: 1
                                    autoReturnCenter: false
                                    isSwidth: true
                                    value: mountIsActive && _activeVehicle.currentMount.mountType===0 && _activeVehicle.currentMount.cam_type === 523?_activeVehicle.currentMount.frame:0
                                    onValueChanged:{
                                        if(mountIsActive && _activeVehicle.currentMount.mountType===0 && _activeVehicle.currentMount.cam_type === 523  && value>=0 && value !== _activeVehicle.currentMount.frame)
                                            _activeVehicle.currentMount.controlFrame(value)
                                    }
                                }
                                QGCLabel{
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    font.pointSize:8
                                    text: qsTr("HD      IR", "高清    红外")
                                }
                            }
                            Column{  //Filr红外色板切换
                                visible: mountIsActive && _activeVehicle.currentMount.mountType===0 && (_activeVehicle.currentMount.cam_type === 443 || _activeVehicle.currentMount.cam_type === 523)
                                MMCSliderBar{
                                    id: _colorPlate
                                    width: parent.width
                                    minimumValue: 0
                                    maximumValue: 2
                                    autoReturnCenter: false
                                    stepSize: 1
                                    isSwidth: true
                                    value: mountIsActive && _activeVehicle.currentMount.mountType===0 && (_activeVehicle.currentMount.cam_type === 443 || _activeVehicle.currentMount.cam_type === 523)?_activeVehicle.currentMount.colorPlate:0
                                    onValueChanged:{
                                        if(mountIsActive && isCompleted && _activeVehicle.currentMount.mountType===0 && (_activeVehicle.currentMount.cam_type === 443 || _activeVehicle.currentMount.cam_type === 523) && value>=0 && value !== _activeVehicle.currentMount.colorPlate)
                                            _activeVehicle.currentMount.controlColorPlate(value)
                                    }
                                }
                                QGCLabel{
                                    anchors.right: parent.right
                                    font.pointSize:8
                                    text: qsTr("BW     WB     RY", "黑白    白黑    红黄")
                                }
                            }

                        }

                        Row{
                            spacing: 20
                            anchors.horizontalCenter: parent.horizontalCenter
                            MMCImageButton {
                                width: 60
                                height: 60
                                sourceWidth: 60
                                sourceHeight: 60
                                checkable: false
                                source:    pressed?"qrc:/qmlimages/takePicture_press.svg":"qrc:/qmlimages/takePicture.svg"
                                backColor: "transparent"
                                overColor: false
                                tooltip: qsTr("Take Pohoto", "点击拍照")
                                onClicked: {
                                    _activeVehicle.doCameraTrigger()
                                }
                            }
                            MMCImageButton {
                                property bool isRecodering: false
                                width: 60
                                height: 60
                                sourceWidth: 60
                                sourceHeight: 60
                                checkable: false
                                source:    {
                                    if(mountIsActive && _activeVehicle.currentMount.mountType===0)
                                        return _activeVehicle.currentMount.videoStatus === 1? pressed?"qrc:/qmlimages/stopVideo_press.svg" : "qrc:/qmlimages/stopVideo.svg":
                                    pressed?"qrc:/qmlimages/takeVideo_press.svg" : "qrc:/qmlimages/takeVideo.svg"
                                    else return pressed?"qrc:/qmlimages/takeVideo_press.svg" : "qrc:/qmlimages/takeVideo.svg"
                                }
                                backColor: "transparent"
                                overColor: false
                                tooltip: qsTr("Record", "录制")
                                onClicked: {
                                    if(mountIsActive)
                                        _activeVehicle.currentMount.videoTape()
                                }
                            }
                        }

                        Column{
                            spacing: 8
                            anchors.horizontalCenter: parent.horizontalCenter
                            Timer{
                                id: checkzoomState
                                interval: 500
                                repeat: false
                                onTriggered: {
                                    if(mountIsActive && win_.visibility)
                                    {
                                        console.log("-------------------------controlZoom(1)");
                                        _activeVehicle.currentMount.controlZoom(1)
                                        checkzoomState.start()
                                    }
                                }
                            }
                            MMCSliderBar{
                                anchors.horizontalCenter: parent.horizontalCenter
                                minimumValue: 1015
                                maximumValue: 2030
                                value: 1523
                                onValueChanged:{
                                    if(!isCompleted || !mountIsActive)
                                        return;
                                    if(value > 1600)
                                    {

                                        if(win_.visibility)
                                                _activeVehicle.currentMount.controlZoom(0)
                                    }
                                    else if(value < 1400){
                                        if(win_.visibility)
                                                _activeVehicle.currentMount.controlZoom(2)
                                    }
                                    if(value == 1523){
                                        if(win_.visibility){
                                                _activeVehicle.currentMount.controlZoom(1)
                                        }
                                    }
                                }
                                onPressedChanged:  {
                                    if(!pressed && autoReturnCenter)
                                    {
                                        value = 1523
                                    }
                                }
                            }
                            Row{
                                anchors.horizontalCenter: parent.horizontalCenter
                                spacing: 30
                                QGCLabel{
                                    font.pointSize: 13
                                    text: "+"
                                }
                                QGCLabel{
                                    font.pointSize: 13
                                    text: qsTr("Zoom", "变焦")
                                }
                                QGCLabel{
                                    font.pointSize: 13
                                    text: "-"
                                }
                            }
                            Row{
                                id: dropRowLayout_
                                spacing: 5
                                height: 25
                                anchors.horizontalCenter: parent.horizontalCenter
                                QGCLabel {
                                    font.pointSize: 10
                                    width: 40
                                    text:qsTr("Pitch:", "俯仰:")
                                }
                                MMCTextField {
                                    width: 50
                                    textColor: "#ffffff"
                                    decimalsnum: 0
                                    horizontalAlignment: TextInput.AlignHCenter
                                    isKeyBoard: false
                                    min: -90
                                    max: 20
                                    text: "0"
                                    onEditingFinished:{
                                        if(mountIsActive)
                                            _activeVehicle.currentMount.controlPitchAngle(text)
                                    }
                                    inputMethodHints: Qt.ImhDialableCharactersOnly
                                    fixed:0

                                }
                                QGCLabel {
                                    font.pointSize: 10
                                    width: 40
                                    text:qsTr("Head:", "偏航:")
                                }
                                MMCTextField {
                                    width: 50
                                    textColor: "#ffffff"
                                    decimalsnum: 0
                                    horizontalAlignment: TextInput.AlignHCenter
                                    isKeyBoard: false
                                    min: -135
                                    max: 135
                                    text: "0"
                                    onEditingFinished:{
                                        if(mountIsActive)
                                            _activeVehicle.currentMount.controlHeadAngle(text)
                                    }
                                    inputMethodHints: Qt.ImhDialableCharactersOnly
                                    fixed:0
                                }
                            }

                            Text {visible:mountIsActive && _activeVehicle.currentMount.cam_type === 491; color: "#fff"; text: qsTr("Tem warning threshold:", "温度告警阈值:")}
                            //Item { height: 10; width: 1 }
                            Row {
                                visible: mountIsActive && _activeVehicle.currentMount.cam_type === 491
                                MMCTextField {
                                    // id: _posHeartbeat
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: 80 //* ScreenTools.widthScale
                                    textColor: "#ffffff"
                                    backColor: "#333"
                                    horizontalAlignment: TextInput.AlignHCenter
                                    text:  mountIsActive  && _activeVehicle.currentMount.cam_type === 491 ? _activeVehicle.currentMount.minTemThreshold : ""
                                    inputMethodHints: Qt.ImhDialableCharactersOnly
                                    onEditingFinished:{
                                        if(mountIsActive)
                                            _activeVehicle.currentMount.minTemThreshold = text
                                    }
                                }
                                Text{anchors.verticalCenter: parent.verticalCenter; color: "#fff"; text: " - "}
                                MMCTextField {
                                    // id: _posHeartbeat
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: 80 //* ScreenTools.widthScale
                                    textColor: "#ffffff"
                                    backColor: "#333"
                                    horizontalAlignment: TextInput.AlignHCenter
                                    text:  mountIsActive && _activeVehicle.currentMount.cam_type === 491 ? _activeVehicle.currentMount.maxTemThreshold : ""
                                    inputMethodHints: Qt.ImhDialableCharactersOnly
                                    onEditingFinished:{
                                        if(mountIsActive)
                                            _activeVehicle.currentMount.maxTemThreshold = text
                                    }
                                }
                            }
                        }
                    }
                }
            }
            Column{
                anchors.verticalCenter: parent.verticalCenter
                Image{
                    id: rightImage
                    source:{
                        var imgUrl;
                        if(!mountIsActive)
                            return ""
                        switch(_activeVehicle.currentMount.cam_type)
                        {
                        case 267://10倍云台
                            imgUrl = "/qmlimages/camera10X.png";
                            break;
                        case 283://20倍松下云台
                            imgUrl = "/qmlimages/camera20X.png";
                            break;
                        case 523://高清双光 20b
                            imgUrl = "qrc:/qmlimages/resources/customItem/PG2IF1-LS1Z20.png";
                            break;
                        case 299://20倍索尼云台
                            break;
                        case 315://30倍奇蛙云台
                            imgUrl = "/qmlimages/camera30X.png";
                            break;
                        case 331://Gopro5云台 - 331
                            imgUrl = "/qmlimages/cameragopro5.png";
                            break;
                        case 316:
                            imgUrl = "/qmlimages/infrared.png";
                            break;
                        case 347://10倍红外双光云台
                            break;
                        case 363://小双光
                            break;
                        case 379://18倍微光
                            imgUrl = "/qmlimages/camera18X.png";
                            break;
                        case 395://35倍微光带跟踪
                            imgUrl = "/qmlimages/camera30X.png"; //camera35X.png;
                            break;
                        case 411://5100云台
                            imgUrl = "/qmlimages/camera5100.png";
                            break;
                        case 427://A7R云台
                            imgUrl = "/qmlimages/cameraA7r.png";
                            break;
                        case 443://Filr红外
                            imgUrl = "/qmlimages/infrared.png"
                            break;
                        case 491://海视英科红外
                            imgUrl = "/qmlimages/pgiy1.png"
                            break;
                        default:
                            imgUrl = "/qmlimages/camera20X.png";
                            break;
                        }
                        return imgUrl; //="/qmlimages/camera20X.png";
                    }
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
        }
        Component.onCompleted: {
            isCompleted = true;
        }
    }
}

