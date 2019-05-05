import QtQuick          2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4

import QGroundControl                       1.0
import QGroundControl.Palette       1.0
import QGroundControl.Controls      1.0


//import QGroundControl.FactSystem    1.0
//import QGroundControl.FactControls  1.0
//import QGroundControl.Palette       1.0
//import QGroundControl.Controls      1.0
//import QGroundControl.ScreenTools   1.0
Item{
    anchors.fill: parent
    anchors.margins: 1
    property var radioMember: QGroundControl.transceiverManager.radioMember
    //    // Live channel monitor control component
    Component {
        id:         monitorColumn
        Item{
            property alias channel : theItem.rcValue
            property alias text: channelLabel.text
            Row {
                anchors.fill: parent
                QGCLabel {
                    id:     channelLabel
                    //                    width: 40
                }
                Item {
                    id:                     theItem
                    anchors.leftMargin:     10
                    anchors.left:           channelLabel.right
                    anchors.verticalCenter: channelLabel.verticalCenter
                    height:                 parent.height
                    width:                  parent.width - channelLabel.width
                    property bool mapped:               true
                    readonly property bool reversed:    false

                    property int            rcValue:      1500
                    property color          __barColor:   qgcPal.window //"#222"
                    property int            _pwmMin:      1000
                    property int            _pwmMax:      2000
                    property int            _pwmRange:    _pwmMax-_pwmMin

                    // Bar
                    Rectangle {
                        id:                     bar
                        anchors.verticalCenter: parent.verticalCenter
                        width:                  parent.width
                        height:                 parent.height / 2
                        color:                  qgcPal.window
                    }

                    // Center point
                    Rectangle {
                        anchors.horizontalCenter:   parent.horizontalCenter
                        width:                      10
                        height:                     parent.height
                        color:                      qgcPal.window
                    }

                    // Indicator
                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width:                  parent.height * 0.75
                        height:                 width
                        x:                      ((Math.abs((theItem.rcValue - theItem._pwmMin) - (theItem.reversed ? theItem._pwmMin : 0)) / theItem._pwmRange) * parent.width) - (width / 2)
                        radius:                 width / 2
                        color:                  qgcPal.text
                        visible:                parent.mapped
                    }

                    QGCLabel {
                        anchors.fill:           parent
                        horizontalAlignment:    Text.AlignHCenter
                        verticalAlignment:      Text.AlignVCenter
                        text:                   "Not Mapped"
                        visible:                !parent.mapped
                    }

                    ColorAnimation {
                        id:         barAnimation
                        target:     bar
                        property:   "color"
                        from:       "yellow"
                        to:         __barColor
                        duration:   1500
                    }
                }
            }

        }
    }

    ExclusiveGroup { id: fenceActionRadioGroup }

    Column {
        width:      parent.width
        spacing:    10

        Column {
            Row{
                QGCButton {
                    visible: 6 <= radioMember.checkStatus || radioMember.checkStatus <= 1
                    text: qsTr("校准")
                    width: 70 * ScreenTools.widgetScale
                    enabled: true
                    onDelayClick: {
                        radioMember.sendCheckStatus()
                    }
                }
                QGCLabel { text:radioMember.calirationState ? qsTr("(已校准)") : qsTr("(未校准)")}
            }

            QGCButton {
                visible: 1 < radioMember.checkStatus && radioMember.checkStatus <= 3
                text: qsTr("下一步")
                width: 70 * ScreenTools.widgetScale
                enabled: true
                onDelayClick: {
                    radioMember.sendCheckStatus()
                }
            }

            QGCButton {
                visible: 3 < radioMember.checkStatus && radioMember.checkStatus <= 5
                text: qsTr("下一步")
                width: 70 * ScreenTools.widgetScale
                enabled: true
                onDelayClick: {
                    radioMember.sendCheckStatus()
                }
            }
            QGCLabel { visible: radioMember.checkStatus === 2;   text: qsTr("请保持摇杆相对禁止，当Mid不变时，点击下一步")}
            QGCLabel { visible: radioMember.checkStatus === 4;   text: qsTr("请最大限制的拨动摇杆，当Min、max不变时，点击下一步") }
            QGCLabel { visible: radioMember.checkStatus === 255; text: qsTr("校准失败") }
            QGCLabel { visible: radioMember.checkStatus === 6;   text: qsTr("校准完成") }
        }

        /* 充电状态，0为不充电，1为充电中 */
        /* 电池电量剩余时间 */
        /* 电池健康状态 */
        /* 电池温度 */
        /* 遥控器开关-是否发送数据给飞控 */
        /* 遥控器模式，左手模式或者右手模式 */
        /* 遥控器校准状态 */
        /* 版本号 */
        QGCLabel { text: radioMember.chargeStateComment + ": " +  radioMember.chargeState }
        QGCLabel { text: "voltage: " + radioMember.voltage }
        QGCLabel { text: "energy: " + radioMember.energy }
        QGCLabel { text: radioMember.timeComment + ": " +  radioMember.time }
        QGCLabel { text: radioMember.stateOfHealthComment + ": " +  radioMember.stateOfHealth }
        QGCLabel { text: radioMember.temperatureComment + ": " +  radioMember.temperature }
        QGCLabel { text: radioMember.rockerStateComment + ": " +  radioMember.rockerState }
        QGCLabel { text: radioMember.rcModeComment + ": " +  radioMember.rcMode }
        QGCLabel { text: radioMember.calirationStateComment + ": " +  radioMember.calirationState }

        QGCLabel { text: radioMember.verComment + ": " +  radioMember.ver }
        Row{
            visible:            true
            spacing:  20
            QGCRadioButton {
                text:               qsTr("Left Model", "左手模式")
                exclusiveGroup:     fenceActionRadioGroup
                checked:            radioMember.rcMode === 0x05
                onClicked: {
                    console.log("--------------------Left Model")
                    radioMember.setCalirationState(true)
                }
            }

            QGCRadioButton {
                text:               qsTr("Right Model", "右手模式")
                exclusiveGroup:     fenceActionRadioGroup
                checked:            radioMember.rcMode === 0x01
                onClicked: {
                    console.log("--------------------Right Model")
                    radioMember.setCalirationState(false)
                }
            }
        }

        QGCLabel { text: qsTr("Channel Monitor") }
        Loader {
            property int channel : radioMember.channel1
            height:                 20
            width:                  500
            sourceComponent:        monitorColumn
            onChannelChanged: item.channel = channel
            Component.onCompleted: {
                item.text = radioMember.channel1Comment
            }
        }

        Loader {
            property int channel : radioMember.channel2
            height:                 20
            width:                  500
            sourceComponent:        monitorColumn
            onChannelChanged: item.channel = channel
            Component.onCompleted: {
                item.text = radioMember.channel2Comment
            }
        }

        Loader {
            property int channel : radioMember.channel3
            height:                 20
            width:                  500
            sourceComponent:        monitorColumn
            onChannelChanged: item.channel = channel
            Component.onCompleted: {
                item.text = radioMember.channel3Comment
            }
        }

        Loader {
            property int channel : radioMember.channel4
            height:                 20
            width:                  500
            sourceComponent:        monitorColumn
            onChannelChanged: item.channel = channel
            Component.onCompleted: {
                item.text = radioMember.channel4Comment
            }

        }

        Column {
            visible: 1 < radioMember.checkStatus && radioMember.checkStatus  < 6

            QGCLabel {
                horizontalAlignment:    Text.AlignHCenter
                verticalAlignment:      Text.AlignVCenter
                text:                   "Min[" + radioMember.channelBMin1 +", " + radioMember.channelBMin2 +", " + radioMember.channelBMin3 +", " + radioMember.channelBMin4  + " ]"
            }

            QGCLabel {
                horizontalAlignment:    Text.AlignHCenter
                verticalAlignment:      Text.AlignVCenter
                text:                   "Mid[" + radioMember.channelBMed1 +", " + radioMember.channelBMed2 +", " + radioMember.channelBMed3 +", " + radioMember.channelBMed4 + " ]"
            }

            QGCLabel {
                horizontalAlignment:    Text.AlignHCenter
                verticalAlignment:      Text.AlignVCenter
                text:                   "Max[" + radioMember.channelBMax1 +", " + radioMember.channelBMax2 +", " + radioMember.channelBMax3 +", " + radioMember.channelBMax4 + " ]"
            }
        }
    }
}
