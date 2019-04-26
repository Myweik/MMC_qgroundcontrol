import QtQuick 2.0
import QtQuick.Layouts 1.2
import QtQuick.Controls.Private 1.0 as QtControlP

import QGroundControl.Controls 1.0
import QGroundControl 1.0
import MMCPMControl 1.0
//PM检测显示区域
Item{
    id: root__
    height: 52
    width: itemRow_.width

    Rectangle{
        anchors.fill: parent
        color: "#222222"
    }
    Row{
        id: itemRow_
        spacing: 0

        Repeater{
            model: MMCPMControl.pmList
            delegate: Item{
                height: 52
                width: 130
                Rectangle{
                    anchors.fill: parent
                    color: "#3e3e3e"
                }
                Item{
                    anchors.left: parent.left
                    anchors.leftMargin: 0
                    width: 2
                    height: 52
                    Rectangle{
                        width: 1
                        height: parent.height
                        anchors.left: parent.left
                        color: "black"
                        opacity: 0.4
                    }
                    Rectangle{
                        width: 1
                        height: parent.height
                        anchors.right: parent.right
                        color: "white"
                        opacity: 0.4
                    }
                }
                ColumnLayout{
                    anchors.left: parent.left
                    anchors.leftMargin: 2
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    QGCLabel{
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: MMCPMControl.pmList.get(index).name
                    }
                    Item{
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        QGCLabel{
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.pointSize: 14
                            text: MMCPMControl.pmList.get(index).value + qsTr("(%1)").arg(MMCPMControl.pmList.get(index).unit)
                        }
                    }
                }
            }
        }

        Item{
            Layout.fillWidth: true
            height: 1
        }
    }
}

