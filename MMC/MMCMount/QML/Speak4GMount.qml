import QtQuick 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0

Item {
    width:row_.width + 20
    height: row_.height + 20
    //4G喊话器
    Row {
        id: row_
        anchors.centerIn: parent
        spacing: 10

        Rectangle{
            height: 290
            width: 250
            color: "#282828"
            Rectangle{
                anchors.fill: parent
                anchors.margins: 1
                border.color: "#4e4d4d"
                color: "#3e3e3e"

                Column {
                    anchors.left:  parent.left
                    anchors.top:  parent.top
                    anchors.right: parent.Right
                    anchors.leftMargin: 15
                    anchors.rightMargin: 15
                    anchors.topMargin: 20

                    Item {
                        width: 149
                        height: 20
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
                                text: mountIsActive ? _srcbItems.get(_activeVehicle.currentMount.playSrc - 1).text : ""
                            }
                        }

                        ComboBox {  //src
                            width: parent.width
                            height: parent.height
                            model: ListModel {
                                id: _srcbItems
                                ListElement { text: qsTr("SD/USB");}
                                ListElement { text: qsTr("TTS");}
                                ListElement { text: qsTr("Police voice play", "警音播放");}
                                ListElement { text: qsTr("External audio", "外部音频");}
                                ListElement { text: qsTr("Digital audio", "数字音频");}
                            }
                            onActivated: if(mountIsActive) _activeVehicle.currentMount.srcSelect(index+1)
                            style: ComboBoxStyle{
                                font.pixelSize: 13
                                background: Item {
                                    Rectangle {
                                        width:  parent.width+2
                                        height: parent.height
                                        color:  "#666"
                                        Image {
                                            id: imageItem
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

                    Item {width:1; height: 25}

                    Item {
                        height: 22
                        width: parent.width
                        QGCLabel{
                            anchors.left:  parent.left
                            font.pointSize: 13
                            text: mountIsActive ? qsTr("Playlist ", "播放曲目 ") + _activeVehicle.currentMount.songNumber : ""
                        }

                        Item { //列表
                            id: _playerList
                            anchors.right: parent.right
                            property bool pressed: false
                            width: 20
                            height: 16
                            Image {
                                anchors.fill: parent
                                source:    _playerList.pressed?"qrc:/qmlimages/resources/customItem/playerItem/playerList.svg":"qrc:/qmlimages/resources/customItem/playerItem/playerList_click.svg"
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    _playerList.pressed = !_playerList.pressed;
                                }
                            }
                        }
                    }

                    Item {width:1; height: 22}

                    Row {
                        spacing: 29
                        MMCImageButton { //上一曲
                            y:5
                            //anchors.horizontalCenter: parent.horizontalCenter
                            //anchors
                            width: 51
                            height: 51
                            sourceWidth: 51
                            sourceHeight: 51
                            checkable: false
                            //rotation: -90
                            source:    pressed?"qrc:/qmlimages/resources/customItem/playerItem/upper.svg":"qrc:/qmlimages/resources/customItem/playerItem/upper_click.svg"
                            backColor: "transparent"
                            overColor: false
                            onPressedChanged:{
                                if(pressed && mountIsActive)
                                    _activeVehicle.currentMount.songUp() //上一曲
                            }
                        }

                        MMCImageButton { //播放 - 停止
                            // anchors.horizontalCenter: parent.horizontalCenter
                            width: 61
                            height: 61
                            sourceWidth: 61
                            sourceHeight: 61
                            checkable: false
                            //rotation: 90
                            source:   mountIsActive && _activeVehicle.currentMount.playState ?
                                          pressed?"qrc:/qmlimages/resources/customItem/playerItem/stop.svg":"qrc:/qmlimages/resources/customItem/playerItem/stop_click.svg"
                            :pressed?"qrc:/qmlimages/resources/customItem/playerItem/play.svg":"qrc:/qmlimages/resources/customItem/playerItem/play_click.svg"
                            backColor: "transparent"
                            overColor: false
                            onPressedChanged:{
                                if(mountIsActive)
                                {
                                    if(_activeVehicle.currentMount.playState){
                                        if(pressed)
                                            _activeVehicle.currentMount.stopPlaying(); //停止播放
                                    }else{
                                        if(pressed)
                                            _activeVehicle.currentMount.startPlaying(); //开始播放
                                    }
                                }
                            }
                        }

                        MMCImageButton { //下一曲
                            //anchors.horizontalCenter: parent.horizontalCenter
                            y:5
                            width: 51
                            height: 51
                            sourceWidth: 51
                            sourceHeight: 51
                            checkable: false
                            //rotation: 90
                            source:    pressed?"qrc:/qmlimages/resources/customItem/playerItem/next.svg":"qrc:/qmlimages/resources/customItem/playerItem/next_click.svg"
                            backColor: "transparent"
                            overColor: false
                            onPressedChanged:{
                                if(pressed && mountIsActive)
                                    _activeVehicle.currentMount.songDown() //下一曲
                            }
                        }

                    }

                    Item {width:1; height: 29}

                    QGCLabel{
                        font.pointSize: 13
                        text: qsTr("volume", "音量")
                    }

                    Item {width:1; height: 19}

                    Column{
                        Row {
                            spacing: 14
                            Image {
                                anchors.verticalCenter: parent.verticalCenter
                                source: "qrc:/qmlimages/resources/customItem/playerItem/volume.svg"
                            }
                            Item {
                                anchors.verticalCenter: parent.verticalCenter
                                width: 193
                                height: 22
                                Rectangle {
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: parent.width
                                    height: 5
                                    radius: 8
                                    color: "gray"
                                }
                                Rectangle {
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: parent.width * (_slider.value - 0.1) * 5 / 4
                                    height: 5
                                    radius: 8
                                    color: "#0e0"
                                }
                                Slider{
                                    id: _slider
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: 193
                                    value: mountIsActive ? _activeVehicle.currentMount.volume/10 : 0.1
                                    minimumValue: 0.1
                                    maximumValue: 0.9
                                    stepSize: 0.1
                                    onValueChanged: {
                                        if(mountIsActive)
                                            _activeVehicle.currentMount.volumeChanged(value)
                                    }
                                    style: SliderStyle{
                                        groove: Item{}
                                        handle: Rectangle {
                                            anchors.centerIn: parent
                                            color: control.pressed ? "white" : "lightgray"
                                            border.color: "#ccc"
                                            border.width: 1
                                            implicitWidth: 14
                                            implicitHeight: 14
                                            radius: 7
                                        }
                                    }
                                }
                            }

                        }
                        Item {
                            width: parent.width
                            height: 22
                            Item { //音量-
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.leftMargin: 33
                                width: 20
                                height: 18
                                Text{
                                    anchors.centerIn: parent
                                    color: "#3f3"
                                    text: "-"
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        if(mountIsActive) _activeVehicle.currentMount.volumeDown(); //音量减
                                    }
                                }
                            }

                            Item{
                                width: 10
                                height: 22
                                anchors.left: parent.left
                                anchors.leftMargin: 122
                                anchors.verticalCenter: parent.verticalCenter
                                //anchors.horizontalCenter: parent.horizontalCenter
                                QGCLabel{
                                    anchors.centerIn: parent
                                    text: mountIsActive ? _activeVehicle.currentMount.volume : ""
                                }
                            }

                            Item { //音量+
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                width: 18
                                height: 18
                                Text{
                                    anchors.centerIn: parent
                                    color: "#3f3"
                                    text: "+"
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        if(mountIsActive) _activeVehicle.currentMount.volumeUp(); //音量加
                                    }
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
                // id: rightImage
                source: "/qmlimages/loud4GSpeaker.png"
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
        Component.onCompleted: {
            isCompleted = true;
        }
    }

}

