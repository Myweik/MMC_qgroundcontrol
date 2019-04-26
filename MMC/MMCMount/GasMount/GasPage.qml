import QtQuick 2.0
import QtQuick.Layouts 1.2
import QtQuick.Controls.Private 1.0 as QtControlP

import QGroundControl.Controls 1.0
import QGroundControl 1.0
import QtMultimedia 5.5

//气体检测显示区域
Item{
    id: root__
    height: 52
    width: itemRow_.width
    readonly property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property var gasList: _activeVehicle && !_activeVehicle.mountLost
                          && _activeVehicle.currentMount && _activeVehicle.currentMount.mountType === 346
                          && _activeVehicle.currentMount.gasList.count > 0 ? _activeVehicle.currentMount.gasList:""

    Rectangle{
        anchors.fill: parent
        color: "#222222"
    }

    Timer{
        id: check_warnTimer
        interval: 5000
        repeat: false
        onTriggered: {
            for(var i=0; i < gasList.count; i++)
            {
                if(gasList.get(i).warningState !== 0)
                {
                    playMusic.source = ""
                    playMusic.source = "/res/audio/warning.wav"
                    playMusic.play()
                    check_warnTimer.start()
                    break
                }
            }
        }
    }

    MediaPlayer {
        id: playMusic
        source: "/res/audio/warning.wav"
        volume: 1.0
    }
    Row{
        id: itemRow_
        spacing: 0
        Item{
            height: parent.height
            width: 34
            Rectangle{
                anchors.fill: parent
                color: "#222222"
            }
            Image{
                anchors.centerIn: parent
                source: "/qmlimages/setup.svg"
            }
            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                onClicked: {
                    warnModel.clear()
                    for(var i=0; i < gasList.count; i++)
                    {
                        warnModel.append({"name": gasList.get(i).name, "uintName":gasList.get(i).uintName,
                                             "warning1Value":gasList.get(i).warning1Value, "warning2Value":gasList.get(i).warning2Value, "warning3Value":gasList.get(i).warning3Value})
                    }
                    setupPage.show()
                }
            }
        }
        ListModel{
            id: warnModel
        }

        GasSetupPage{
            id: setupPage
            gasList: root__.gasList
            warnList: warnModel
        }
        Grid{
            columns: 5
            Repeater{
                model: gasList
                delegate: Item{
                    height: 52
                    width: 152
                    property var warnState: gasList.get(index).warningState
                    onWarnStateChanged: {
                        //                    console.log("----------onWarnStateChanged", warnState, "check timer running", check_warnTimer.running)
                        if(warnState !== 0 && !check_warnTimer.running)
                        {
                            check_warnTimer.start()
                            playMusic.source = ""
                            playMusic.source = "/res/audio/warning.wav"
                            playMusic.play()
                        }
                        else if(warnState === 0)
                        {
                            for(var i=0; i < gasList.count; i++)
                            {
                                if(gasList.get(i).warningState !== 0)
                                    return;
                            }
                            playMusic.stop()
                        }
                    }

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
                        RowLayout{
                            Layout.leftMargin: 5
                            spacing: 5
                            QGCLabel{
                                text: gasList.get(index).name + ":"
                            }
                            Rectangle{
                                visible: gasList.get(index).name !== "O2"
                                width: 12
                                height: width
                                radius: width/2
                                border.color: "black"
                                color: gasList.get(index).warningState === 0?"#29b0f2":"#3e3e3e"
                                MouseArea{
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered: {
                                        QtControlP.Tooltip.showText(parent.parent, Qt.point(parent.x + parent.width, parent.y), qsTr("Normal Concentration:", "正常浓度"))
                                    }
                                    onExited: {
                                        QtControlP.Tooltip.hideText()
                                    }
                                }
                            }
                            Rectangle{
                                visible: gasList.get(index).name !== "O2"
                                width: 12
                                height: width
                                border.width: 1
                                border.color: "black"
                                radius: width/2
                                color: gasList.get(index).warningState === 1?"#d9e021":"#3e3e3e"
                                MouseArea{
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered: {
                                        QtControlP.Tooltip.showText(parent.parent, Qt.point(parent.x + parent.width, parent.y), qsTr("Short time allowed to contact：", "短时间允许接触浓度：") + gasList.get(index).warning1Value + gasList.get(index).uintName)
                                    }
                                    onExited: {
                                        QtControlP.Tooltip.hideText()
                                    }
                                }
                            }
                            Rectangle{
                                visible: gasList.get(index).name !== "O2"
                                width: 12
                                height: width
                                border.width: 1
                                border.color: "black"
                                radius: width/2
                                color: gasList.get(index).warningState === 2?"#fbb03b":"#3e3e3e"
                                MouseArea{
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered: {
                                        QtControlP.Tooltip.showText(parent.parent, Qt.point(parent.x + parent.width, parent.y), qsTr("Damage Concentration：", "伤害浓度:") + gasList.get(index).warning2Value + gasList.get(index).uintName)
                                    }
                                    onExited: {
                                        QtControlP.Tooltip.hideText()
                                    }
                                }
                            }
                            Rectangle{
                                width: 12
                                height: width
                                visible: gasList.get(index).name !== "O2" && gasList.get(index).name !== "EX"
                                border.width: 1
                                border.color: "black"
                                radius: width/2
                                color: gasList.get(index).warningState === 3?"#ff0000":"#3e3e3e"
                                MouseArea{
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered: {
                                        QtControlP.Tooltip.showText(parent.parent, Qt.point(parent.x + parent.width, parent.y), qsTr("Semi-lethal Concentration：", "半致死浓度：") + gasList.get(index).warning3Value + gasList.get(index).uintName)
                                    }
                                    onExited: {
                                        QtControlP.Tooltip.hideText()
                                    }
                                }
                            }
                        }
                        Item{
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            QGCLabel{
                                anchors.horizontalCenter: parent.horizontalCenter
                                font.pointSize: 14
                                text: gasList.get(index).concentration.toFixed(2) + qsTr("(%1)").arg(gasList.get(index).uintName)
                                color:{
                                    if(gasList.get(index).name === "O2")
                                        return "#29b0f2";
                                    switch(gasList.get(index).warningState)
                                    {
                                    case 0:
                                        return "#29b0f2";
                                    case 1:
                                        return "#d9e021";
                                    case 2:
                                        return "#fbb03b";
                                    case 3:
                                        return "#ff0000";
                                    }
                                }
                            }
                            MouseArea{
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: {
                                    QtControlP.Tooltip.showText(parent.parent, Qt.point(parent.x + parent.width/2, parent.y), gasList.get(index).warningState > 0?qsTr("Level %1 Alarm", "%1级报警").arg(gasList.get(index).warningState):qsTr("Normal Concentration", "正常浓度"))
                                }
                                onExited: {
                                    QtControlP.Tooltip.hideText()
                                }
                            }
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

