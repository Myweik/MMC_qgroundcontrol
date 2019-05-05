/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


import QtQuick                  2.2
import QtQuick.Controls         1.2
import QtQuick.Controls.Styles  1.2

import QGroundControl                       1.0
import QGroundControl.FactSystem            1.0
import QGroundControl.Controls              1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.MultiVehicleManager   1.0
import QGroundControl.Palette               1.0

Item{
    width: _keySettingRoot.width + 40
    height: _keySettingRoot.height + 10

    Column {
        id:             _keySettingRoot
        anchors.left: parent.Left
        anchors.leftMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 10
        spacing: 15

        Item {
            height: 30
            width: 185
            Text{
                anchors.centerIn: parent
                color: "#fff" //"#9BACBE"
                font.pointSize: 18
                font.bold:  true
                text: qsTr("自定义按键配置：", "自定义按键配置：")
            }
        }

        Row {
            spacing: 10
            Item {
                anchors.verticalCenter: anchors.verticalCenter
                height: 30
                width: 120
                Text{
                    anchors.centerIn: parent
                    color: "#fff" //"#9BACBE"

                    font.pixelSize: 14
                    text: qsTr("自定义按键1：", "自定义按键1：")
                }
            }
            MMCComboBox {
                id: customKey1
                property int key: 4
                anchors.verticalCenter: anchors.verticalCenter
                //                pointSize: 10
                textColor: "#fff"
                fontColor: textColor
//                backColor: "transparent"
                enabled:   true
                model: model
                width: 120
                height: 30
                fontSize: 14
                isElide:false
                onActivated: {
                    if (index != -1) {
                        currentIndex = index
                    }
                }

                onCurrentIndexChanged:{
                    QGroundControl.transceiverManager.radioMember.keyBinding(key, model.get(currentIndex).modelData)
                }

                Component.onCompleted: {
                    /* - 加载信息 - */
                    var value =  QGroundControl.transceiverManager.radioMember.getKeyValue(key);
                    if(value !== ""){
                        var index = customKey1.find(value)
                        if (index !== -1) {
                            customKey1.currentIndex = index
                            customKey1.currentText = value
                        }
                    }
                }
            }
        }

        Row {
            spacing: 10
            Item {
                anchors.verticalCenter: anchors.verticalCenter
                height: 30
                width: 120
                Text{
                    anchors.centerIn: parent
                    color: "#fff"

                    font.pixelSize: 14
                    text: qsTr("自定义按键2：", "自定义按键2：")
                }
            }

            MMCComboBox {
                id: customKey2
                property int key: 3
                anchors.verticalCenter: anchors.verticalCenter
                textColor: "#fff"
                fontColor: textColor
//                backColor: "transparent"
                enabled:   true
                model: model
                width: 120
                height: 30
                fontSize: 14
                isElide:false
                onCurrentIndexChanged:{
                    QGroundControl.transceiverManager.radioMember.keyBinding(key, model.get(currentIndex).modelData)
                }

                Component.onCompleted: {
                    /* - 加载串口信息 - */
                    var value =  QGroundControl.transceiverManager.radioMember.getKeyValue(key);
                    if(value !== ""){
                        var index = customKey2.find(value)
                        if (index !== -1) {
                            customKey2.currentIndex = index
                            customKey2.currentText = value
                        }
                    }
                }
            }
        }
    }

    ListModel { id: model }

    Component.onCompleted: {
        model.clear()
        //        // Add new items
        var data
        for (var i = 0; i < QGroundControl.transceiverManager.radioMember.keyModes.length; i++) {
            data = {'modelData': QGroundControl.transceiverManager.radioMember.keyModes[i] }; //"Finger tracking"
            model.append(data);
        }
    }
}

