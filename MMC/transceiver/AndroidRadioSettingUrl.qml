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

QGCView {
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
            Column {
                id:                 settingsColumn
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
                                width:      ScreenTools.defaultFontPixelWidth
                                spacing:    10

                                Column {
                                    Row{
                                        QGCButton {
                                            visible: 6 <= radioMember.checkStatus || radioMember.checkStatus <= 1
                                            text: qsTr("校准")
                                            width: 70 * ScreenTools.widgetScale
                                            enabled: true
                                            onClicked: {
                                                radioMember.sendCheckStatus()
                                            }
                                        }
                                        QGCLabel {
                                            text: radioMember.calirationState ? qsTr("(已校准)") : qsTr("(未校准)")
                                            color : radioMember.calirationState ? "#0f0" : "#f00"
                                        }
                                    }

                                    QGCButton {
                                        visible: 1 < radioMember.checkStatus && radioMember.checkStatus <= 3
                                        text: qsTr("下一步")
                                        width: 70 * ScreenTools.widgetScale
                                        enabled: true
                                        onClicked: {
                                            radioMember.sendCheckStatus()
                                        }
                                    }

                                    QGCButton {
                                        visible: 3 < radioMember.checkStatus && radioMember.checkStatus <= 5
                                        text: qsTr("下一步")
                                        width: 70 * ScreenTools.widgetScale
                                        enabled: true
                                        onClicked: {
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

                                ExclusiveGroup { id: fenceActionRadioGroup }
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



//                            FactCheckBox {
//                                text:       qsTr("Enable Microhard")
//                                fact:       _microhardEnabledFact
//                                enabled:    true
//                                visible:    _microhardEnabledFact.visible
//                            }

                    }
                }



                //-----------------------------------------------------------------
                //-- Connection Status
//                Item {
//                    width:                      _panelWidth
//                    height:                     statusLabel.height
//                    anchors.margins:            ScreenTools.defaultFontPixelWidth
//                    anchors.horizontalCenter:   parent.horizontalCenter
//                    visible:                    _microhardEnabled
//                    QGCLabel {
//                        id:                     statusLabel
//                        text:                   qsTr("Connection Status")
//                        font.family:            ScreenTools.demiboldFontFamily
//                    }
//                }
//                Rectangle {
//                    height:                     statusCol.height + (ScreenTools.defaultFontPixelHeight * 2)
//                    width:                      _panelWidth
//                    color:                      qgcPal.windowShade
//                    visible:                    _microhardEnabled
//                    anchors.margins:            ScreenTools.defaultFontPixelWidth
//                    anchors.horizontalCenter:   parent.horizontalCenter
//                    Column {
//                        id:                     statusCol
//                        spacing:                ScreenTools.defaultFontPixelHeight * 0.5
//                        width:                  parent.width
//                        anchors.centerIn:       parent
//                        GridLayout {
//                            anchors.margins:    ScreenTools.defaultFontPixelHeight
//                            columnSpacing:      ScreenTools.defaultFontPixelWidth * 2
//                            anchors.horizontalCenter: parent.horizontalCenter
//                            columns: 2
//                            QGCLabel {
//                                text:           qsTr("Ground Unit:")
//                                Layout.minimumWidth: _labelWidth
//                            }
//                            QGCLabel {
//                                text:           QGroundControl.microhardManager.connected ? qsTr("Connected") : qsTr("Not Connected")
//                                color:          QGroundControl.microhardManager.connected ? qgcPal.colorGreen : qgcPal.colorRed
//                                Layout.minimumWidth: _valueWidth
//                            }
//                            QGCLabel {
//                                text:           qsTr("Air Unit:")
//                            }
//                            QGCLabel {
//                                text:           QGroundControl.microhardManager.linkConnected ? qsTr("Connected") : qsTr("Not Connected")
//                                color:          QGroundControl.microhardManager.linkConnected ? qgcPal.colorGreen : qgcPal.colorRed
//                            }
//                            QGCLabel {
//                                text:           qsTr("Uplink RSSI:")
//                            }
//                            QGCLabel {
//                                text:           QGroundControl.microhardManager.linkConnected && QGroundControl.microhardManager.uplinkRSSI < 0 ? QGroundControl.microhardManager.uplinkRSSI : ""
//                            }
//                            QGCLabel {
//                                text:           qsTr("Downlink RSSI:")
//                            }
//                            QGCLabel {
//                                text:           QGroundControl.microhardManager.linkConnected && QGroundControl.microhardManager.downlinkRSSI < 0 ? QGroundControl.microhardManager.downlinkRSSI : ""
//                            }
//                        }
//                    }
//                }
//                //-----------------------------------------------------------------
//                //-- IP Settings
//                Item {
//                    width:                      _panelWidth
//                    height:                     ipSettingsLabel.height
//                    anchors.margins:            ScreenTools.defaultFontPixelWidth
//                    anchors.horizontalCenter:   parent.horizontalCenter
//                    visible:                    _microhardEnabled
//                    QGCLabel {
//                        id:                     ipSettingsLabel
//                        text:                   qsTr("Network Settings")
//                        font.family:            ScreenTools.demiboldFontFamily
//                    }
//                }
//                Rectangle {
//                    height:                     ipSettingsCol.height + (ScreenTools.defaultFontPixelHeight * 2)
//                    width:                      _panelWidth
//                    color:                      qgcPal.windowShade
//                    visible:                    _microhardEnabled
//                    anchors.margins:            ScreenTools.defaultFontPixelWidth
//                    anchors.horizontalCenter:   parent.horizontalCenter
//                    Column {
//                        id:                     ipSettingsCol
//                        spacing:                ScreenTools.defaultFontPixelHeight * 0.5
//                        width:                  parent.width
//                        anchors.centerIn:       parent
//                        GridLayout {
//                            anchors.margins:    ScreenTools.defaultFontPixelHeight
//                            columnSpacing:      ScreenTools.defaultFontPixelWidth * 2
//                            anchors.horizontalCenter: parent.horizontalCenter
//                            columns: 2
//                            QGCLabel {
//                                text:           qsTr("Local IP Address:")
//                                Layout.minimumWidth: _labelWidth
//                            }
//                            QGCTextField {
//                                id:             localIP
//                                text:           QGroundControl.microhardManager.localIPAddr
//                                enabled:        true
//                                inputMethodHints:    Qt.ImhFormattedNumbersOnly
//                                Layout.minimumWidth: _valueWidth
//                            }
//                            QGCLabel {
//                                text:           qsTr("Remote IP Address:")
//                            }
//                            QGCTextField {
//                                id:             remoteIP
//                                text:           QGroundControl.microhardManager.remoteIPAddr
//                                enabled:        true
//                                inputMethodHints:    Qt.ImhFormattedNumbersOnly
//                                Layout.minimumWidth: _valueWidth
//                            }
//                            QGCLabel {
//                                text:           qsTr("Ground Unit IP Address:")
//                                Layout.minimumWidth: _labelWidth
//                            }
//                            QGCTextField {
//                                id:             groundIP
//                                text:           QGroundControl.microhardManager.groundIPAddr
//                                enabled:        true
//                                inputMethodHints:    Qt.ImhFormattedNumbersOnly
//                                Layout.minimumWidth: _valueWidth
//                            }
//                            QGCLabel {
//                                text:           qsTr("Air Unit IP Address:")
//                            }
//                            QGCTextField {
//                                id:             airIP
//                                text:           QGroundControl.microhardManager.airIPAddr
//                                enabled:        true
//                                inputMethodHints:    Qt.ImhFormattedNumbersOnly
//                                Layout.minimumWidth: _valueWidth
//                            }
//                            QGCLabel {
//                                text:           qsTr("Network Mask:")
//                            }
//                            QGCTextField {
//                                id:             netMask
//                                text:           QGroundControl.microhardManager.netMask
//                                enabled:        true
//                                inputMethodHints:    Qt.ImhFormattedNumbersOnly
//                                Layout.minimumWidth: _valueWidth
//                            }
//                            QGCLabel {
//                                text:           qsTr("Configuration password:")
//                            }
//                            QGCTextField {
//                                id:             configPassword
//                                text:           QGroundControl.microhardManager.configPassword
//                                enabled:        true
//                                inputMethodHints:    Qt.ImhHiddenText
//                                Layout.minimumWidth: _valueWidth
//                            }
//                            QGCLabel {
//                                text:           qsTr("Encryption key:")
//                            }
//                            QGCTextField {
//                                id:             encryptionKey
//                                text:           QGroundControl.microhardManager.encryptionKey
//                                enabled:        true
//                                inputMethodHints:    Qt.ImhHiddenText
//                                Layout.minimumWidth: _valueWidth
//                            }
//                        }
//                        Item {
//                            width:  1
//                            height: ScreenTools.defaultFontPixelHeight
//                        }
//                        QGCButton {
//                            function validateIPaddress(ipaddress) {
//                                if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ipaddress))
//                                    return true
//                                return false
//                            }
//                            function testEnabled() {
//                                if(localIP.text          === QGroundControl.microhardManager.localIPAddr &&
//                                    remoteIP.text        === QGroundControl.microhardManager.remoteIPAddr &&
//                                    groundIP.text        === QGroundControl.microhardManager.groundIPAddr &&
//                                    airIP.text           === QGroundControl.microhardManager.airIPAddr &&
//                                    netMask.text         === QGroundControl.microhardManager.netMask &&
//                                    configPassword.text  === QGroundControl.microhardManager.configPassword &&
//                                    encryptionKey.text   === QGroundControl.microhardManager.encryptionKey)
//                                    return false
//                                if(!validateIPaddress(localIP.text))  return false
//                                if(!validateIPaddress(remoteIP.text)) return false
//                                if(!validateIPaddress(groundIP.text)) return false
//                                if(!validateIPaddress(airIP.text)) return false
//                                if(!validateIPaddress(netMask.text))  return false
//                                return true
//                            }
//                            enabled:            testEnabled()
//                            text:               qsTr("Apply")
//                            anchors.horizontalCenter:   parent.horizontalCenter
//                            onClicked: {
//                                QGroundControl.microhardManager.setIPSettings(localIP.text, remoteIP.text, groundIP.text, airIP.text, netMask.text, configPassword.text, encryptionKey.text)
//                            }

//                        }
//                    }
//                }
            }
        }
    }
}
