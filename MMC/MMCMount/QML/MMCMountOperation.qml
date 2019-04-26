import QtQuick 2.5
import QtQuick.Layouts 1.2
import QGroundControl.Controls 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4



Item {
    id: _MainItem
    width: mainShow.width
    height: mainShow.height
    property int type: 1

    //挂载是否存在，判断条件
    property bool mountIsActive: _activeVehicle && !_activeVehicle.connectionLost && !_activeVehicle.mountLost && _activeVehicle.currentMount
    property var mountType: _activeVehicle && !_activeVehicle.mountLost && _activeVehicle.currentMount?_activeVehicle.currentMount.mountType:-1 
    property var cam_type: mountType === 0 ?  _activeVehicle.currentMount.cam_type :-1
    property int current_cam_type: -1

    onMountTypeChanged: {
        switch(mountType)
        {
        case -1:
            return;
        case 0:
            mountLoader.source = "MMCCloudTerrace.qml"
            break;
        case 298:
            mountLoader.source = "MMCDropMount.qml"
            break;
        case 266:
            mountLoader.source = "MMCSpeakMount.qml"
            break;
//        case 362:
//            mountLoader.sourceComponent = speak4GMount
//            break
        case 346:
            mountLoader.source = "MMCGasMount.qml"
            break;
        case 282:
            mountLoader.source = "MMCLightMount.qml"
            break;
        case 284:
            mountLoader.source = "MMCCamera3dmapMount.qml"
            break;
        default:
            mountLoader.source = ""
            break;
        }
    }

    onCam_typeChanged: {
        if(current_cam_type === cam_type) return;
        current_cam_type = cam_type;

        model.clear();
        var data ;
        if(cam_type !== -1){
            /* 指点移动 */
            data = {'modelData': qsTr("Move", "指点移动"), 'key': 1}; //"Finger movement"
            model.append(data);
        }
        if(cam_type === 443 || cam_type === 523 || cam_type === 507 || cam_type === 491){
            /* 指点测温 */
            data = {'modelData': qsTr("Temperature(P)", "指点测温"), 'key': 2};  //"Finger thermometric"
            model.append(data);
        }
        if(cam_type === 395){
            /* 指点跟踪 */
            data = {'modelData': qsTr("Follow", "指点跟踪"), 'key': 3}; //"Finger tracking"
            model.append(data);
        }
        if(cam_type === 523){
            /* 指点跟踪 */
            data = {'modelData': qsTr("Temperature(A)", "区域测温"), 'key': 4}; //"Finger tracking"
            model.append(data);
        }
        if(cam_type === -1) type = 1;
        else _directionsCombox.currentIndex = 0;
    }

    onVisibleChanged: {
        _directionsTimer.start()
    }

    ColumnLayout {
        id: mainShow
        spacing: 0

        Rectangle{  //折叠区
            Layout.fillWidth: true
            Layout.minimumWidth: 80
            height: 35
            color:  "#222"

            QGCLabel{
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 5
                font.pointSize: 10
                color:"#8796B0"
                text: {
                    if(_activeVehicle && !_activeVehicle.mountLost && _activeVehicle.currentMount)
                    {
                        if(_activeVehicle.currentMount.mountType === 0)
                            return _activeVehicle.currentMount.mountName(_activeVehicle.currentMount.cam_type)
                        else
                            return _activeVehicle.currentMount.mountName(_activeVehicle.currentMount.mountType)
                    }
                    return ""
                }
            }

            MMCImageButton {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 5
                width: 20
                height: 20
                sourceWidth: 20
                sourceHeight: 20
                checkable: false
                source:    !pressed?"/qmlimages/fold.svg":"/qmlimages/fold_click.svg"
                backColor: "transparent"
                overColor: false
                onClicked: {
                    showSwitcher()
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: showSwitcher()
            }

            Rectangle{
                anchors.top: parent.bottom
                width: parent.width
                height: 2
                color: "#111"
            }
        }

        Item { //ComBox -- 指点控制
            Layout.fillWidth: true
            height: 35
            visible: model.count > 1

            Timer {
                id: _directionsTimer
                interval : 100
                repeat : false
                running : false
                triggeredOnStart : false
                onTriggered: _directionsCombox.enabled = _MainItem.visible
            }

            MMCComboBox {
                id: _directionsCombox
                anchors.fill: parent
                pointSize: 10
                textColor: "#8796B0"
                fontColor: textColor
                backColor: "transparent"
                enabled:     false

                property bool controlFlow: false
                fontSize: 13
                model: model
                onCurrentIndexChanged:{
                    type = model.get(currentIndex).key;
                        if(type !== 3){
                           if(controlFlow)  _activeVehicle.currentMount.controlFlow(false) // 关闭跟踪模式
                           controlFlow = false;
                        }else{
                            controlFlow = true;
                            _activeVehicle.currentMount.controlFlow(true) // 开启跟踪模式
                        }
                }
                ListModel { id: model }
            }
        }
        Rectangle{
            visible: model.count > 1
            Layout.fillWidth: true
            height: 2
            color: "#111"
        }
        Loader{ id: mountLoader }
    }
}
