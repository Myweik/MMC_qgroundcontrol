import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Controllers   1.0
import MMCSettings 1.0

Window {
    id: win_
    flags : Qt.Window | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowSystemMenuHint |Qt.WindowCloseButtonHint //只显示关闭

    readonly property var  _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle
    property bool isCompleted: false
    //挂载是否存在，判断条件
    property bool mountIsActive: _activeVehicle && !_activeVehicle.connectionLost && !_activeVehicle.mountLost && _activeVehicle.currentMount
    property var mountType: mountIsActive ? _activeVehicle.currentMount.mountType : -1

    //通用挂载
    property bool mountWindowActive: false;

    //透传相机切换
    property bool pixelType : true //true - 4K/25Hz  false - 1080P/50Hz  //多处统一用一个
    function pixelTypeWitch(){
        var text;
        if(pixelType){  //true - 4K/25Hz
            text = "55aada03ffa7800303"
        }else{          //false - 1080P/50Hz
            text = "55aada03ffa08002fc"
        }
        pixelType = !pixelType //来自父对象ID
//        console.log("---------------codeComboBox_", text)
        _activeVehicle.currentMount.controlCommand(text)
    }
    function customMountChe(){
        mountLoader.sourceComponent = customMount
        close()
    }

    //刚构造100毫秒内不许叉掉界面
    property bool immortalState : true
    Timer{
        id: _immortaltimer
        interval: 100
        repeat : false
        running: false
        triggeredOnStart: false
        onTriggered: {
            win_.immortalState = false
        }
    }
    onVisibleChanged: {
        if(visible) {
            _haidtimer.start()
            _immortaltimer.start()
        }else _haidtimer.stop()
    }
    onClosing:{
        if(win_.immortalState){
            close.accepted = false
        }else{
            mountWindowActive = false
            win_.immortalState = true
        }
    }

    Timer {
        id: _haidtimer
        interval: 200
        repeat : true
        running: false
        onTriggered: {
            if(win_.visibility  && _activeVehicle && _activeVehicle.isControlMount === 1)
                close()
        }
    }
    width:         mountLoader.item? (mountLoader.item.width  + 40) : 40
    height:        mountLoader.item? (mountLoader.item.height + 40) : 40
    minimumHeight: mountLoader.item? (mountLoader.item.height + 40) : 40
    minimumWidth:  mountLoader.item? (mountLoader.item.width  + 40) : 40
    maximumHeight: mountLoader.item? (mountLoader.item.height + 40) : 40
    maximumWidth:  mountLoader.item? (mountLoader.item.width  + 40) : 40

    title: qsTr("Mount Control", "挂载控制")
    color: "#3e3e3e"
    Rectangle{
        anchors.fill: parent
        color: "#282828"
    }

    onMountTypeChanged: {
        //close()
        switch(mountType)
        {
//        case -1:
//            return;
        case 0:
            mountLoader.source = "qrc:/qml/CameraMount.qml"
            break;
        case 298:
            mountLoader.source = "qrc:/qml/DropMount.qml"
            break;
        case 266:
            mountLoader.source = "qrc:/qml/SpeakMount.qml"
            break;
        case 362:
            mountLoader.source = "qrc:/qml/Speak4GMount.qml"
            break
        case 346:
            mountLoader.source = "qrc:/qml/GasMount.qml";
            break;
        case 282:
            mountLoader.source = "qrc:/qml/LightMount.qml"
            break;
        case 284:
            mountLoader.source = "qrc:/qml/Camera3dmapMount.qml"
            break;
        default:
            mountLoader.source = "qrc:/qml/CustomMount.qml"
            break;
        }
    }

    //help  -- 帮助文档
    Rectangle{
        id:helpdocumentInmount
        width: 28
        height: 25
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        visible: true
        color: Qt.rgba(Math.random(),Math.random(),Math.random(),0)
        Image {
            anchors.fill: parent
            id: helpSvg
            source: "qrc:/qmlimages/resources/helpMount.svg"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
                helpdocumentInmountLoader.source = ""
                helpdocumentInmountLoader.source = "qrc:/qml/src/help/help.qml"
                if(mountIsActive){
                    var path = "qrc:/qml/src/help/mountcontrol.html"
                    if(_activeVehicle.currentMount.mountType !== 0){
                        path = MMCSettings.getApplicationDirPath() + "/pack/MountHelp/" + _activeVehicle.currentMount.mountMode(_activeVehicle.currentMount.mountType) + ".html"

                    }else{
                        path = MMCSettings.getApplicationDirPath() + "/pack/MountHelp/" +  _activeVehicle.currentMount.mountMode(_activeVehicle.currentMount.cam_type) + ".html"
                    }
                    helpdocumentInmountLoader.item.fileUrl = Qt.resolvedUrl(path)
                }else{
                    helpdocumentInmountLoader.item.fileUrl = "qrc:/qml/src/help/mountcontrol.html"
                }
            }
        }
    }
    Loader{
        id:helpdocumentInmountLoader
    }

    Loader{
        anchors.centerIn: parent
        id: mountLoader
        source:""
    }
}
