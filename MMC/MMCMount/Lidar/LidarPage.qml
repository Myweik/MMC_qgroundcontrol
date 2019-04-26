import QtQuick 2.0
import QtQuick.Layouts 1.2
import QtQuick.Controls.Private 1.0 as QtControlP

import QGroundControl.Controls 1.0
import QGroundControl 1.0
import QtMultimedia 5.5

//激光雷达显示区域
Item{
    id: root__
    height: 52
    width: itemRow_.width
    readonly property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property var lidarList: _activeVehicle?_activeVehicle.lidarList:""

    Rectangle{
        anchors.fill: parent
        color: "#222222"
    }
    Row{
        id: itemRow_
        spacing: 0
        Repeater{
            model: lidarList
            delegate: Item{
                height: 52
                width: 80
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
                    anchors.fill: parent
                    QGCLabel{
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pointSize: 12
                        text: index * 45 + "°"
                        color: "white"
                    }
                    QGCLabel{
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pointSize: 16
                        text: lidarList.get(index).distance.toFixed(2) + "m"
                        color: _lidarDistance/*/100*/ < lidarList.get(index).distance ? "white":"red"
                    }
                }
            }
        }
    }
}

