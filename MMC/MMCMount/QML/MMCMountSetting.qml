import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Controllers   1.0
import QGroundControl.ScreenTools   1.0

import QtQuick          2.4
import QtQuick.Dialogs  1.2
import QtLocation       5.3
import QtPositioning    5.3
import QtQuick.Layouts  1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0
import QtGraphicalEffects 1.0
import QtMultimedia 5.5

import QGroundControl.FlightMap     1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.Palette       1.0
import QGroundControl.Mavlink       1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0
import QGroundControl.MMCMounts      1.0
import MMCPMControl 1.0
import FireRec      1.0
import MMCSettings  1.0
import ScreenParam  1.0
import JsonRevise       1.0
Window {
    id: win_
    width: debugPageLoader.item.width + 40
    height: debugPageLoader.item.height + 40
    title: qsTr("Debug Online", "在线调试")
    color: "#3e3e3e"

    //挂载是否存在，判断条件
    property bool mountIsActive: _activeVehicle && !_activeVehicle.connectionLost && !_activeVehicle.mountLost && _activeVehicle.currentMount
    property var mountType: mountIsActive ? _activeVehicle.currentMount.mountType : -1

    //云台类才有调参
    property bool writeRomState: mountType === 0 ? _activeVehicle.currentMount.writeROMState : false
    property bool writeFlashState: mountType === 0 ? _activeVehicle.currentMount.writeFlashState : false

    property int currentType: 0

    property string writeRomLabelText: " "
    property string writeFlashLabelText: " "

    Timer {
        id: writeRamTimer
        interval: 500;
        onTriggered: writeRomLabelText = " "
    }

    Timer {
        id: writeFlashTimer
        interval: 500;
        onTriggered: writeFlashLabelText = " "
    }

    onWriteRomStateChanged:
    {
//        console.log("onWriteRomStateChanged" ,writeRomState)
        writeRamTimer.running = true
        writeRomLabelText = "Write RAM OK!"

    }

    onWriteFlashStateChanged:
    {
//        console.log("onWriteFlashStateChanged" ,writeFlashState)
        writeFlashTimer.running = true
        writeFlashLabelText = "Write Flash OK!"
    }

    Rectangle{
        anchors.fill: parent
        color: "#282828"
    }
    function debugTypeName(index){
        switch(index)
        {
        case 0:
            return "姿态环P:"
        case 1:
            return "姿态环I"
        case 2:
            return "姿态环D"
        case 3:
            return "姿态积分限幅"
        case 4:
            return "姿态输出限幅"
        case 5:
            return "速度环P"
        case 6:
            return "速度环I"
        case 7:
            return "速度环D"
        case 8:
            return "速度积分限幅"
        case 9:
            return "速度输出限幅"
        default:
            break;
        }
    }

    function m1_pid_type(index){
        switch(index)
        {
        case 0:
            currentType = 0
            return 1
        case 1:
            currentType = 3
            return 3
        case 2:
            currentType = 4
            return 5
        case 3:
            return 7
        case 4:
            return 9
        case 5:
            return 2
        case 6:
            return 4
        case 7:
            return 6
        case 8:
            return 8
        case 9:
            return 10
        default:
            break;
        }
    }
    function m2_pid_type(index){
        switch(index)
        {
        case 0:
            return 11
        case 1:
            return 13
        case 2:
            return 15
        case 3:
            return 17
        case 4:
            return 19
        case 5:
            return 12
        case 6:
            return 14
        case 7:
            return 16
        case 8:
            return 18
        case 9:
            return 20
        default:
            break;
        }
    }

    function m3_pid_type(index){
        switch(index)
        {
        case 0:
            return 21
        case 1:
            return 23
        case 2:
            return 25
        case 3:
            return 27
        case 4:
            return 29
        case 5:
            return 22
        case 6:
            return 24
        case 7:
            return 26
        case 8:
            return 28
        case 9:
            return 30
        default:
            break;
        }
    }

     Loader{
        id: debugPageLoader
        sourceComponent: debugOnlineDlg
    }

     ExclusiveGroup{
         id: exculsive
     }

    //在线调试
    property Component debugOnlineDlg:Column{
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 20
        anchors.topMargin: 20
        spacing: 10
        Row{
            id: _loadFlashRow
            spacing: 15
            QGCLabel {
                font.pointSize: 15
                text: qsTr("Request Data:", "请求数据:")
            }

            //Request Data
            MMCImageButton{
                borderWidth: 1
                borderColor: hovered?"#00cc00":"#ffffff"
                checkable: false
                overColor: false
                width: 28 * ScreenTools.heightScale
                height: 28 * ScreenTools.heightScale
                sourceWidth: 28 * ScreenTools.heightScale - 5
                sourceHeight: 28 * ScreenTools.heightScale - 5
                source: "/resources/guided/down.svg"
                exclusiveGroup: exculsive
                onClicked:{
                    if(mountType === 0)
                        _activeVehicle.currentMount.readPIDdata(1)
//                    console.log("Request Data")
                }
            }//end mmcimage

            QGCLabel {
                font.pointSize: 15
                text: qsTr("Load to Flash:", "写入Flash:")
            }

            //upload Flash
            MMCImageButton{
                borderWidth: 1
                borderColor: hovered?"#00cc00":"#ffffff"
                checkable: false
                overColor: false
                width: 28 * ScreenTools.heightScale
                height: 28 * ScreenTools.heightScale
                sourceWidth: 28 * ScreenTools.heightScale - 5
                sourceHeight: 28 * ScreenTools.heightScale - 5
                source: "/resources/guided/up.svg"
                exclusiveGroup: exculsive
                onClicked:{
                    if(mountType === 0)
                        _activeVehicle.currentMount.readPIDdata(0)
//                    console.log("update Flash")
                }
            }//end mmcimage

            QGCLabel {
                id:_writeRomLabel
                font.pointSize: 15
                visible: writeRomState
                text: writeRomLabelText
            }

            QGCLabel {
                id:_writeFlashLabel
                font.pointSize: 15
                visible: writeFlashState
                text: writeFlashLabelText
            }
        }

        Item{
            width: parent.width - 50
            height: 2
            Rectangle{
                anchors.top: parent.top
                width: parent.width
                height: 1
                color: "#191919"
            }
            Rectangle{
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: "#595757"
            }
        }

        Row{
            spacing: 10

            Column{
                QGCLabel{
                    width: 120
                    text: ""
                }
                Repeater{
                    model: 10
                    Row{
                        spacing: 15
                        height: 30

                        QGCLabel{
                            width: 120
                            text: debugTypeName(index)
                        }
                     }
                }
             }

            //M1
            Column{
                id: _m1column
                QGCLabel{
                    width: 120
                    text: mountType === 0 ? "俯仰" : "NULL"
                }
                Repeater{
                    id: _m1Repeater
                    model: mountType === 0 ? _activeVehicle.currentMount.dataPIDList : 0
                        Row{
                            spacing: 15
                            height: 30
                            QGCTextField {
                                id: _textField
                                text: modelData
                                showUnits:  true
                                inputMethodHints: Qt.ImhDigitsOnly
                                onEditingFinished: {
                                    if(mountType === 0){
                                        _activeVehicle.currentMount.writePIDdata(m1_pid_type(index),text)
                                        modelData = text
                                    }
                                }
                            }

                            //upload RAM
                            MMCImageButton{
                                borderWidth: 1
                                borderColor: hovered?"#00cc00":"#ffffff"
                                checkable: false
                                overColor: false
                                width: 28 * ScreenTools.heightScale
                                height: 28 * ScreenTools.heightScale
                                sourceWidth: 28 * ScreenTools.heightScale - 5
                                sourceHeight: 28 * ScreenTools.heightScale - 5
                                source: "/qmlimages/firmwareStart.svg"
                                exclusiveGroup: exculsive
                                onClicked:{
                                    if(mountType === 0)
                                        _activeVehicle.currentMount.writePIDdata(m1_pid_type(index),_textField.text)

                                }
                            }//end mmcimage
                        }
                    }//end repeat
                }//end column

            //M2
            Column{
                QGCLabel{
                    width: 120
                    text: "横滚"
                }
                Repeater{
                    model: mountType === 0 ?  _activeVehicle.currentMount.dataPIDListM2 : 0
                        Row{
                            spacing: 15
                            height: 30
                            QGCTextField {
                                id: _textField2
                                text: modelData
//                                unitsLabel: qsTr("Step+%1", "步进%1:").arg(stepNum(index))//missionItem.gridAngle.units
                                showUnits:  true
                                onEditingFinished: {
                                    if(mountType === 0)
                                    {
                                        _activeVehicle.currentMount.writePIDdata(m2_pid_type(index),text)
                                        modelData = text
                                    }
                                }
                            }

                            //upload RAM
                            MMCImageButton{
                                borderWidth: 1
                                borderColor: hovered?"#00cc00":"#ffffff"
                                checkable: false
                                overColor: false
                                width: 28 * ScreenTools.heightScale
                                height: 28 * ScreenTools.heightScale
                                sourceWidth: 28 * ScreenTools.heightScale - 5
                                sourceHeight: 28 * ScreenTools.heightScale - 5
                                source: "/qmlimages/firmwareStart.svg"
                                exclusiveGroup: exculsive
                                onClicked:{
//                                    console.log("update2 RAM" ,index,m2_pid_type(index),modelData,_textField2.text)
                                    if(mountType === 0)
                                        _activeVehicle.currentMount.writePIDdata(m2_pid_type(index),_textField2.text)

                                }
                            }//end mmcimage
                        }
                    }
                }//end column
            //M3
            Column{
                QGCLabel{
                    width: 120
                    text: "航向"
                }
                Repeater{
                    model: mountType === 0 ? _activeVehicle.currentMount.dataPIDListM3 : 0
                        Row{
                            spacing: 15
                            height: 30

                            QGCTextField {
                                id: _textField3
                                text: modelData //_activeVehicle.currentMount.dataOnline(index)//missionItem.gridAngle.value
//                                unitsLabel: qsTr("Step+%1", "步进%1:").arg(stepNum(index))//missionItem.gridAngle.units
                                showUnits:  true
                                onEditingFinished: {
//                                    console.log("QGCTextField3" , m3_pid_type(index),text)
                                    if(mountType === 0)
                                    {
                                        _activeVehicle.currentMount.writePIDdata(m3_pid_type(index),text)
                                        modelData = text
                                    }
                                }
                            }

                            //upload RAM
                            MMCImageButton{
                                borderWidth: 1
                                borderColor: hovered?"#00cc00":"#ffffff"
                                checkable: false
                                overColor: false
                                width: 28 * ScreenTools.heightScale
                                height: 28 * ScreenTools.heightScale
                                sourceWidth: 28 * ScreenTools.heightScale - 5
                                sourceHeight: 28 * ScreenTools.heightScale - 5
                                source: "/qmlimages/firmwareStart.svg"
                                exclusiveGroup: exculsive
                                onClicked:{
//                                    console.log("update3 RAM", index ,m3_pid_type(index),modelData,_textField3.text)
                                    if(mountType === 0)
                                        _activeVehicle.currentMount.writePIDdata(m3_pid_type(index),text)
                                }
                            }//end mmcimage
                        }
                    }
                }//end column
        }

       Column{
            ListModel {
                id: model_
            }

            Row{
                spacing: 15
                ComboBox{
                    id: codeComboBox_
                    width: 400
                    height: 30
                    editable: true
                    model: model_
                }
                function contain(text){
                    for(var i = 0; i < model_.count; i++){
                        if(model_.get(i).text === text) return true;
                    }
                    return false;
                }
                MMCTextImageBtn{
                    imageEnable: false
                    width:         60
                    text:          qsTr("发送", "Send")
                    height:        30
                    backColor:   "#191919"
                    sourceColor: hovered?"#00cc00":"#999"
                    borderColor: hovered?"#00cc00":"#999"
                    checkable: true
                    onClicked:{
                        if(codeComboBox_.editText != "" && mountType === 0)
                        {
                            if(!parent.contain(codeComboBox_.editText))
                                model_.append({text:codeComboBox_.editText})
                            _activeVehicle.currentMount.controlCommand(codeComboBox_.editText)
                        }
                    }
                }
            }

//            QGCLabel{
//                width: 120
//                text: mountType === 0 ? activeVehicle.currentMount.backCode:""
//            }
        }
     }
}

