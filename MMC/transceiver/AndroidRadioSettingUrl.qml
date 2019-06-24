/****************************************************************************
 *
 *   (c) 2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


import QtGraphicalEffects       1.0
import QtMultimedia             5.5
import QtQuick                  2.3
import QtQuick.Controls         1.2
import QtQuick.Controls.Styles  1.4
import QtQuick.Dialogs          1.2
import QtQuick.Layouts          1.2
import QtLocation               5.3
import QtPositioning            5.3

import QGroundControl                       1.0
import QGroundControl.Controllers           1.0
import QGroundControl.Controls              1.0
import QGroundControl.FactControls          1.0
import QGroundControl.FactSystem            1.0
import QGroundControl.Palette               1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.SettingsManager       1.0

QGCView /*Rectangle*/ {
    id:                 _qgcView
    viewPanel:          panel
    color:              qgcPal.window
    anchors.fill:       parent
    anchors.margins:    ScreenTools.defaultFontPixelWidth

    property real _labelWidth:                  ScreenTools.defaultFontPixelWidth * 26
    property real _valueWidth:                  ScreenTools.defaultFontPixelWidth * 20
    property real _panelWidth:                  _qgcView.width * _internalWidthRatio

    //    property Fact _microhardEnabledFact:        QGroundControl.settingsManager.appSettings.enableMicrohard
    //    property bool _microhardEnabled:            _microhardEnabledFact.rawValue

    readonly property real _internalWidthRatio:          0.8

    property var radioMember: QGroundControl.transceiverManager.radioMember
    property var fpvMember: QGroundControl.transceiverManager.fpvMember

    property bool vehicleArmed:                 _activeVehicle ? _activeVehicle.armed : false

    QGCPalette { id: qgcPal }

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
                    width:  parent.width * 0.3
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
                    property color          __barColor:   /*qgcPal.window */ "#222"
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
                        to:         theItem.__barColor
                        duration:   1500
                    }
                }
            }
        }
    }

    QGCViewPanel {
        id:             panel
        anchors.fill:   parent

        QGCFlickable {
            clip:               true
            anchors.fill:       parent
            contentHeight:      settingsColumn.height
            contentWidth:       settingsColumn.width

            Column{
                id:                 settingsColumn
                Column {
                    width:              _qgcView.width
                    spacing:            ScreenTools.defaultFontPixelHeight * 0.5
                    anchors.margins:    ScreenTools.defaultFontPixelWidth
                    //-----------------------------------------------------------------
                    //-- General
                    Item {
                        width:                      _panelWidth
                        height:                     generalLabel.height
                        anchors.margins:            ScreenTools.defaultFontPixelWidth
                        anchors.horizontalCenter:   parent.horizontalCenter
                        QGCLabel {
                            id:             generalLabel
                            text:           qsTr("General")
                            font.family:    ScreenTools.demiboldFontFamily
                        }
                    }
                    Rectangle {
                        height:                     generalRow.height + (ScreenTools.defaultFontPixelHeight * 2)
                        width:                      _panelWidth
                        color:                      qgcPal.windowShade
                        anchors.margins:            ScreenTools.defaultFontPixelWidth
                        anchors.horizontalCenter:   parent.horizontalCenter

                        Row {
                            id:                 generalRow
                            spacing:            ScreenTools.defaultFontPixelWidth * 4
                            anchors.centerIn:   parent

                            Column {
                                spacing:    10

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
                                //                                QGCLabel { text: radioMember.rockerStateComment + ": " +  radioMember.rockerState }
                                QGCLabel { text: radioMember.rcModeComment + ": " +  radioMember.rcMode }
                                QGCLabel { text: radioMember.calirationStateComment + ": " +  radioMember.calirationState }

                                QGCLabel { text: radioMember.verComment + ": " +  radioMember.ver }

                                Row{
                                    visible:            true
                                    spacing:  20
                                    ExclusiveGroup { id: fenceActionRadioGroup }
                                    QGCRadioButton {
                                        exclusiveGroup:     fenceActionRadioGroup
                                        text:               qsTr("Left Model", "左手模式")
                                        checked:            radioMember.rcMode === 0x05
                                        onClicked: {
                                            console.log("--------------------Left Model")
                                            if(checked)
                                                radioMember.setCalirationState(true)
                                        }
                                    }

                                    QGCRadioButton {
                                        exclusiveGroup:     fenceActionRadioGroup
                                        text:               qsTr("Right Model", "右手模式")
                                        checked:            radioMember.rcMode === 0x0A
                                        onClicked: {
                                            console.log("--------------------Right Model")
                                            if(checked)
                                                radioMember.setCalirationState(false)
                                        }
                                    }
                                }

                                Column {
                                    visible: !vehicleArmed
                                    Row{
                                        spacing:  10
                                        visible: 6 <= radioMember.checkStatus || radioMember.checkStatus <= 1
                                        QGCButton {
                                            text: qsTr("Calibration", "校准")
                                            width: 70 * ScreenTools.widgetScale
                                            enabled: true
                                            onClicked: {
                                                radioMember.sendCheckStatus()
                                            }
                                        }
                                        QGCLabel {
                                            text: radioMember.calirationState ? qsTr("(OK)", "(已校准)") : qsTr("(NG)", "(未校准)")
                                            color : radioMember.calirationState ? "#0f0" : "#f00"
                                        }
                                    }

                                    QGCButton {
                                        visible: 1 < radioMember.checkStatus && radioMember.checkStatus <= 3
                                        text: qsTr("Next", "下一步")
                                        width: 70 * ScreenTools.widgetScale
                                        enabled: true
                                        onClicked: {
                                            radioMember.sendCheckStatus()
                                        }
                                    }

                                    QGCButton {
                                        visible: 3 < radioMember.checkStatus && radioMember.checkStatus <= 5
                                        text: qsTr("Next", "下一步")
                                        width: 70 * ScreenTools.widgetScale
                                        enabled: true
                                        onClicked: {
                                            radioMember.sendCheckStatus()
                                        }
                                    }
                                    QGCLabel { visible: radioMember.checkStatus === 2;   text: qsTr("Keep the rocker relatively banned.
        When Mid remains unchanged, click Next", "请保持摇杆相对禁止，
        当Mid不变时，点击下一步")}
                                    QGCLabel { visible: radioMember.checkStatus === 4;   text: qsTr("Turn the rocker to the maximum limit.
        When Min and Max remain unchanged, click Next.", "请最大限制的拨动摇杆，
        当Min、max不变时，点击下一步") }
                                    QGCLabel { visible: radioMember.checkStatus === 255; text: qsTr("Calibration failed", "校准失败") }
                                    QGCLabel { visible: radioMember.checkStatus === 6;   text: qsTr("Calibration completed", "校准完成") }
                                }

                                Column {
                                    visible: 1 < radioMember.checkStatus && radioMember.checkStatus  < 6

                                    QGCLabel {
                                        horizontalAlignment:    Text.AlignHCenter
                                        verticalAlignment:      Text.AlignVCenter
                                        text:                   "Min[" + radioMember.channelBMin1 +", " + radioMember.channelBMin2 +", " + radioMember.channelBMin3 +", " + radioMember.channelBMin4 +", " + radioMember.channelBMin7 +", " + radioMember.channelBMin8 + " ]"
                                    }

                                    QGCLabel {
                                        horizontalAlignment:    Text.AlignHCenter
                                        verticalAlignment:      Text.AlignVCenter
                                        text:                   "Mid[" + radioMember.channelBMid1 +", " + radioMember.channelBMid2 +", " + radioMember.channelBMid3 +", " + radioMember.channelBMid4 +", " + radioMember.channelBMid7 +", " + radioMember.channelBMid8 + " ]"
                                    }

                                    QGCLabel {
                                        horizontalAlignment:    Text.AlignHCenter
                                        verticalAlignment:      Text.AlignVCenter
                                        text:                   "Max[" + radioMember.channelBMax1 +", " + radioMember.channelBMax2 +", " + radioMember.channelBMax3 +", " + radioMember.channelBMax4 +", " + radioMember.channelBMax7 +", " + radioMember.channelBMax8 + " ]"
                                    }

                                    QGCLabel {
                                        horizontalAlignment:    Text.AlignHCenter
                                        verticalAlignment:      Text.AlignVCenter
                                        text:                   "Ver[" + radioMember.channelBVer1 +", " + radioMember.channelBVer2 +", " + radioMember.channelBVer3 +", " + radioMember.channelBVer4 +", " + radioMember.channelBVer7 +", " + radioMember.channelBVer8 + " ]"
                                    }
                                }
                            }

                            Column {
                                spacing:    10
                                QGCLabel { text: qsTr("Channel Monitor") }
                                Loader {
                                    property int channel : radioMember.channel1
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel1Comment
                                        item.channel = channel
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
                                        item.channel = channel
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
                                        item.channel = channel
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
                                        item.channel = channel
                                    }
                                }

                                Loader {
                                    property int channel : radioMember.channel5
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel5Comment
                                        item.channel = channel
                                    }
                                }

                                Loader {
                                    property int channel : radioMember.channel6
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel6Comment
                                        item.channel = channel
                                    }
                                }

                                Loader {
                                    property int channel : radioMember.channel7
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel7Comment
                                        item.channel = channel
                                    }
                                }

                                Loader {
                                    property int channel : radioMember.channel8
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel8Comment
                                        item.channel = channel
                                    }
                                }

                                Loader {
                                    property int channel : radioMember.channel9
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel9Comment
                                        item.channel = channel
                                    }
                                }

                                Loader {
                                    property int channel : radioMember.channel10
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel10Comment
                                        item.channel = channel
                                    }
                                }

                                Loader {
                                    property int channel : radioMember.channel11
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel11Comment
                                        item.channel = channel
                                    }
                                }

                                Loader {
                                    property int channel : radioMember.channel12
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel12Comment
                                        item.channel = channel
                                    }
                                }

                                Loader {
                                    property int channel : radioMember.channel13
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel13Comment
                                        item.channel = channel
                                    }
                                }

                                Loader {
                                    property int channel : radioMember.channel14
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel14Comment
                                        item.channel = channel
                                    }
                                }

                                Loader {
                                    property int channel : radioMember.channel15
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel15Comment
                                        item.channel = channel
                                    }
                                }
                                Loader {
                                    property int channel : radioMember.channel16
                                    height:                 20
                                    width:                  500
                                    sourceComponent:        monitorColumn
                                    onChannelChanged: item.channel = channel
                                    Component.onCompleted: {
                                        item.text = radioMember.channel16Comment
                                        item.channel = channel
                                    }
                                }
                            }
                        }
                    }
                }

                Column {
                    width:              _qgcView.width
                    spacing:            ScreenTools.defaultFontPixelHeight * 0.5
                    anchors.margins:    ScreenTools.defaultFontPixelWidth
                    //-----------------------------------------------------------------
                    //-- General
                    Item {
                        width:                      _panelWidth
                        height:                     generalLabel.height
                        anchors.margins:            ScreenTools.defaultFontPixelWidth
                        anchors.horizontalCenter:   parent.horizontalCenter
                        QGCLabel {
                            text:           qsTr("GT Config")
                            font.family:    ScreenTools.demiboldFontFamily
                        }
                    }

                    Rectangle {
                        height:                     generalColumn.height + (ScreenTools.defaultFontPixelHeight * 2)
                        width:                      _panelWidth
                        color:                      qgcPal.windowShade
                        anchors.margins:            ScreenTools.defaultFontPixelWidth
                        anchors.horizontalCenter:   parent.horizontalCenter


                        Column {
                            id:generalColumn
                            spacing:            ScreenTools.defaultFontPixelWidth * 4
                            anchors.centerIn:   parent

                            Row{
                                visible: !vehicleArmed
                                anchors.horizontalCenter:   parent.horizontalCenter
                                spacing:  10
                                QGCButton {
                                    text: qsTr("pair", "pair")
                                    width: 70 * ScreenTools.widgetScale
                                    enabled: !fpvMember.pairStatus
                                    onClicked: {
                                        fpvMember.setConfig()
                                    }
                                }
                                QGCLabel {
                                    visible: fpvMember.pairStatus
                                    text:  qsTr("(pairing...)", "(pairing...)")
                                }
                            }

                            Row{
                                anchors.horizontalCenter:   parent.horizontalCenter
                                spacing:  30
                                QGCLabel {
                                    text: fpvMember.RC === 0 ? qsTr("(RC:---)") : qsTr("(RC:") + fpvMember.RC + "%)"
                                }

                                QGCLabel {
                                    text: fpvMember.VT === 0  ? qsTr("(VT:---)") : qsTr("(VT:") + fpvMember.VT + "%)"
                                }
                            }
                        }
                    }
                }
            }

        } // QGCFlickable
    } // QGCViewPanel

}
