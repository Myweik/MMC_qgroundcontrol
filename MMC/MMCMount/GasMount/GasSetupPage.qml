import QtQuick.Window 2.2
import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

import QGroundControl.Controls 1.0

Window{
    title: "气体警报值设置"
    modality: Qt.ApplicationModal
    flags: Qt.Dialog
    width: 550
    height: 480
    maximumHeight: 480
    maximumWidth: 550
    minimumHeight: 480
    minimumWidth: 550
    color: "#222222"
    property var gasList
    property var warnList
    QGCFlickable{
        anchors.fill: parent
        clip: true
        contentHeight: column_.width + 40
        Column{
            id: column_
            anchors.fill: parent
            anchors.margins: 10
            spacing: 20
            Column{
                spacing: 10
                Repeater{
                    model: warnList
                    RowLayout{
                        height: 90
                        spacing: 15
                        Item{
                            height: 30
                            width: 80
                            Column{
                                anchors.centerIn: parent
                                QGCLabel{
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: name
                                    onTextChanged:{
                                        warning1.text = warnList.get(index).warning1Value.toFixed(2)
                                        warning2.text = warnList.get(index).warning2Value.toFixed(2)
                                        warning3.text = warnList.get(index).warning3Value.toFixed(2)
                                    }
                                }
                                QGCLabel{
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    font.pointSize: 10 * ScreenTools.widgetScale
                                    text: "(" + uintName + ")"
                                }
                            }
                        }
                        Column{
                            spacing: 10
                            Row{
                                spacing: 5
                                QGCLabel{
                                    width: 180
                                    text: "短时间允许接触浓度:"
                                }
                                MMCTextField{
                                    id: warning1
                                    width: 100
                                    textColor: "#ffffff"
                                    onTextChanged: {
                                        warnList.get(index).warning1Value = text
                                    }
                                }
                            }
                            Row{
                                spacing: 5
                                QGCLabel{
                                    width: 180
                                    text: "伤害浓度:"
                                }
                                MMCTextField{
                                    id: warning2
                                    width: 100
                                    textColor: "#ffffff"
                                    onTextChanged: {
                                        warnList.get(index).warning2Value = text
                                    }
                                }
                            }
                            Row{
                                spacing: 5
                                QGCLabel{
                                    width: 180
                                    text: "半致死浓度:"
                                }
                                MMCTextField{
                                    id: warning3
                                    width: 100
                                    textColor: "#ffffff"
                                    onTextChanged: {
                                        warnList.get(index).warning3Value = text
                                    }
                                }
                            }
                        }
                        Rectangle{
                            width: 100
                            height: 30
                            border.color: "white"
                            border.width: 1
                            color: "#222222"
                            QGCLabel{
                                anchors.centerIn: parent
                                font.pointSize: 16
                                text: "恢复默认"
                            }
                            MouseArea{
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton
                                onClicked: {
                                    gasList.get(index).reset()
                                    warning1.text = gasList.get(index).warning1Value.toFixed(2)
                                    warning2.text = gasList.get(index).warning2Value.toFixed(2)
                                    warning3.text = gasList.get(index).warning3Value.toFixed(2)
                                    warning1Value = gasList.get(index).warning1Value
                                    warning2Value = gasList.get(index).warning2Value
                                    warning3Value = gasList.get(index).warning3Value
                                }
                                onDoubleClicked: {
                                    gasList.get(index).reset()
                                    warning1.text = gasList.get(index).warning1Value.toFixed(2)
                                    warning2.text = gasList.get(index).warning2Value.toFixed(2)
                                    warning3.text = gasList.get(index).warning3Value.toFixed(2)
                                    warning1Value = gasList.get(index).warning1Value
                                    warning2Value = gasList.get(index).warning2Value
                                    warning3Value = gasList.get(index).warning3Value
                                }
                            }
                        }
                    }
                }
            }
            Row{
                anchors.right: parent.right
                anchors.rightMargin: 20
                height: 30
                spacing: 20
                Rectangle{
                    width: 100
                    height: 30
                    border.color: "white"
                    border.width: 1
                    color: "#222222"
                    QGCLabel{
                        anchors.centerIn: parent
                        font.pointSize: 16
                        text: "应用"
                    }
                    MouseArea{
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked: {
                            for(var i = 0; i < gasList.count; i++)
                            {
                                for(var j=0; j < warnList.count; j++)
                                {
                                    if(warnList.get(j).name === gasList.get(i).name)
                                    {
                                        gasList.get(i).warning1Value = warnList.get(j).warning1Value
                                        gasList.get(i).warning2Value = warnList.get(j).warning2Value
                                        gasList.get(i).warning3Value = warnList.get(j).warning3Value
                                    }
                                }
                            }
                            close()
                        }
                        onDoubleClicked: {
                            for(var i = 0; i < gasList.count; i++)
                            {
                                for(var j=0; j < warnList.count; j++)
                                {
                                    if(warnList.get(j).name === gasList.get(i).name)
                                    {
                                        gasList.get(i).warning1Value = warnList.get(j).warning1Value
                                        gasList.get(i).warning2Value = warnList.get(j).warning2Value
                                        gasList.get(i).warning3Value = warnList.get(j).warning3Value
                                    }
                                }
                            }
                            close()
                        }
                    }
                }
                Rectangle{
                    width: 100
                    height: 30
                    border.color: "white"
                    border.width: 1
                    color: "#222222"
                    QGCLabel{
                        anchors.centerIn: parent
                        font.pointSize: 16
                        text: "取消"
                    }
                    MouseArea{
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked: {
                            close()
                        }
                        onDoubleClicked: {
                            close()
                        }
                    }
                }
            }
        }
    }
}
