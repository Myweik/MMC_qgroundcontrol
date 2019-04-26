import QtQuick          2.4
import QtQuick.Controls 1.4

import QGroundControl               1.0
//import MMCSettings  1.0
//import QGroundControl.MMCMounts 1.0

import QGroundControl.Controls      1.0
import QtQuick.Layouts 1.2
import QGroundControl.ScreenTools   1.0
Item{
    anchors.fill: parent
    id: _control
    property int type: 1

//    MMCMountControl {
//        id: _control
//        z: parent.z + 5
//        anchors.top: parent.top
//        anchors.right: parent.right
//        anchors.rightMargin: 15
//        visible: true// !_mainIsMap && _activeVehicle  && _fullParameterVehicleAvailable
//        source: currentMountImg()
//    }

    QGCToolBarButton {
        z: multiArea.z + 1
        anchors.top:        parent.top
        anchors.left:       parent.left
        width:              120
        height:             120
        logo:               true
//        exclusiveGroup:     mainActionGroup
        source:             "/qmlimages/PaperPlane.svg"
        onClicked:          QGroundControl.videoManager.fullScreen = !QGroundControl.videoManager.fullScreen
    }

    Rectangle{
        id: flowArea_
        border.color: "yellow"
        border.width: 2
        color: "transparent"
        visible: true
    }

    Timer{
        id: hideFlowArea_
        interval: 1000
        repeat: false
        onTriggered: {
            flowArea_.visible = false
        }
    }

    MultiPointTouchArea{  //手势
        id: multiArea
        visible:  _activeVehicle && !_activeVehicle.mountLost && _activeVehicle.currentMount
        anchors.fill: parent
        property bool isPressed: false
        property int guesture_Type:0   //当前手势的类型 1:放大  2：缩小   0：无
        z: parent.z + 1
        mouseEnabled: true
        minimumTouchPoints: 2
        maximumTouchPoints: 2
        onUpdated: {
            if(control_Area.isPressed && control_Area.posiTionChanged)
                return;
                        console.log("-----------------MultiPointTouchArea onUpdated", touchPoints.length)
            if(touchPoints.length === 2)
            {
                var curDistance = Math.sqrt((touchPoints[1].x - touchPoints[0].x)*(touchPoints[1].x - touchPoints[0].x) + (touchPoints[1].y - touchPoints[0].y)*(touchPoints[1].y - touchPoints[0].y))
                var oldDistance = Math.sqrt((touchPoints[1].startX - touchPoints[0].startX)*(touchPoints[1].startX - touchPoints[0].startX) + (touchPoints[1].startY - touchPoints[0].startY)*(touchPoints[1].startY - touchPoints[0].startY))
                if(curDistance - oldDistance > 50)
                    guesture_Type = 1
                else if(curDistance - oldDistance < -50)
                    guesture_Type = 2
                else
                    guesture_Type = 0
//                            console.log("-----------------distance ", curDistance, oldDistance, guesture_Type)
            }
            // 消除最后的release 防止进行点击进行指点操作
            interval_delay.start()
        }
        onCanceled: {
                        console.log("-----------------MultiPointTouchArea onCanceled")
            isPressed = false;
            _activeVehicle.currentMount.controlZoom(1)
            guesture_Type = 0
        }

        onPressed: {
            if(control_Area.isPressed && control_Area.posiTionChanged)
                return;
            isPressed = true;
        }
        onReleased: {
            if(control_Area.isPressed && control_Area.posiTionChanged)
                return;
            isPressed = false;
            _activeVehicle.currentMount.controlZoom(1)
            guesture_Type = 0
        }
        // 定时检查手势
        Timer{
            id: checkGestureTimer_
            interval: 100
            repeat: true
            running: multiArea.isPressed
            onTriggered: {
                if(!multiArea.isPressed)
                    return;
                if(multiArea.guesture_Type == 1)
                {
                    _activeVehicle.currentMount.controlZoom(2)
                }
                else if(multiArea.guesture_Type == 2)
                {
                    _activeVehicle.currentMount.controlZoom(0)
                }
            }
        }

        MouseArea{
            id: control_Area
            property bool isPressed: false
            property bool posiTionChanged: false
            property bool isPressAndHold:false
            property var oldPos:Qt.point(0, 0)
            property var curPos:Qt.point(0, 0)


            propagateComposedEvents: true

            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            onDoubleClicked: {
                mouse.accepted = false
            }

            onPressed: {
                if(multiArea.isPressed)
                    return;
                isPressed = true
                oldPos.x= mouse.x
                oldPos.y= mouse.y
                posiTionChanged=false
            }
            onReleased: {
                console.log("-------------onReleased", flowArea_.width, flowArea_.height, control_Area.posiTionChanged)
                if(multiArea.isPressed)
                    return;
                isPressed = false
                if(_control.type === 3 && flowArea_.visible)
                {
                    if(control_Area.posiTionChanged/* && _activeVehicle.currentMount.cam_type === 395*/)
                        calPos2(flowArea_.x, flowArea_.y, flowArea_.width, flowArea_.height)
                    hideFlowArea_.start()
                }else if( _control.type === 4 && flowArea_.visible)
                {
                    if(control_Area.posiTionChanged/* && _activeVehicle.currentMount.cam_type === 395*/)
                        calPos2(flowArea_.x, flowArea_.y, flowArea_.width, flowArea_.height)
                    hideFlowArea_.start()
                }
                else{
                    console.log("-------------onReleased");

                    if(_activeVehicle &&  _activeVehicle.mountLost && !_activeVehicle.currentMount
                            && (_activeVehicle.currentMount.mountType !==284 || _activeVehicle.currentMount.mountType === 362 /*|| _activeVehicle.currentMount.mountType === 1*/))//倾斜 4G喊话 未知 没有此功能
                        activeVehicle.currentMount.controlPitch(0, false)

                    if(_activeVehicle &&  _activeVehicle.mountLost && !_activeVehicle.currentMount /*&& _activeVehicle.currentMount.mountType ===0*/)//非云台类 没有此功能
                        activeVehicle.currentMount.controlHead(0, false)
                }

            }
            onClicked: {
                console.log("------------------onclicked", _control.type)
                if(multiArea.isPressed)
                    return;
                if(interval_delay.running)
                    return;
                interval_delay.start()
                if(!posiTionChanged)
                {
                    if(_control.type === 3 || _control.type === 4)
                    {
                        var imgWidth = 1920.000
                        var imgHeight = 1080.000
                        var scal
                        if(imgWidth/_flightVideo.width > imgHeight/_flightVideo.height)
                            scal = imgWidth/_flightVideo.width
                        else
                            scal = imgHeight/_flightVideo.height
                        if(!control_Area.posiTionChanged /*&& _activeVehicle.currentMount.cam_type === 395*/)
                            calPos2(mouse.x - 50/scal, mouse.y- 50/scal, 100/scal, 100/scal)
                        posiTionChanged=false
                        return;
                    }
                    else
                        calPos(mouse.x, mouse.y, _control.type)
                }
                posiTionChanged=false
            }

            onPositionChanged: {
                console.log("----------------------------onPositionChanged")
                if(multiArea.isPressed)
                    return;
                curPos.x = mouse.x
                curPos.y = mouse.y
                if(Math.abs(control_Area.curPos.x - control_Area.oldPos.x) > 10 || Math.abs(control_Area.curPos.y - control_Area.oldPos.y) > 10)
                    posiTionChanged = true
                else
                    posiTionChanged = false

                if(_control.type === 3 || _control.type === 4)
                {
                    if(mouse.x - control_Area.oldPos.x > 0 && mouse.y - control_Area.oldPos.y > 0)
                    {
                        flowArea_.x = oldPos.x
                        flowArea_.y = oldPos.y
                        var imgWidth = 1920.000
                        var imgHeight = 1080.000
                        var scal
                        if(imgWidth/_flightVideo.width > imgHeight/_flightVideo.height)
                            scal = imgWidth/_flightVideo.width
                        else
                            scal = imgHeight/_flightVideo.height

                        if((mouse.x - control_Area.oldPos.x)> 234/scal && _control.type === 3)  //存在出界bug
                            flowArea_.width = 234/scal
                        else
                            flowArea_.width = mouse.x - control_Area.oldPos.x

                        if((mouse.y - control_Area.oldPos.y) > 131/scal && _control.type === 3)
                            flowArea_.height = 131/scal
                        else
                            flowArea_.height = mouse.y - control_Area.oldPos.y

                        flowArea_.visible = true
                    }
                    else{
                        flowArea_.visible = false
                    }
                    if(control_Timer.running)
                        control_Timer.stop();
                }
                else{
                    if(control_Timer.running)
                        return;
                    control_Timer.start()
                }
            }

            //计算屏幕触摸的坐标投影到真实画幅的坐标，以中心点位原点计算
            function calPos(x,y,type)
            {
                if(!_activeVehicle || _activeVehicle.mountLost || !_activeVehicle.currentMount || _activeVehicle.currentMount.mountType !==0)
                    return; //非云台类没有此功能

                console.log("-----------------calPos1", x, y, type)
                var retPosX, retPosY
                var imgWidth = 1920.000
                var imgHeight = 1080.000
                var curWidth=_flightVideo.width/2
                var curHeight=_flightVideo.height/2
                var scal
                if(imgWidth/_flightVideo.width > imgHeight/_flightVideo.height)
                    scal = imgWidth/_flightVideo.width
                else
                    scal = imgHeight/_flightVideo.height

                retPosX = (x - curWidth) * scal
                retPosY = (curHeight - y) * scal
                if(Math.abs(retPosX) > imgWidth/2 || Math.abs(retPosY) > imgHeight/2)
                    return;
                _activeVehicle.currentMount.controlPos(retPosX, retPosY,type)
            }

            function calPos2(x,y,width,height)
            {
                if(!_activeVehicle || _activeVehicle.mountLost || !_activeVehicle.currentMount || _activeVehicle.currentMount.mountType !==0)
                    return; //非云台类没有此功能

                var retPosX, retPosY
                var imgWidth = 1920.000
                var imgHeight = 1080.000
                var curWidth=_flightVideo.width/2
                var curHeight=_flightVideo.height/2
                var scal
                if(imgWidth/_flightVideo.width > imgHeight/_flightVideo.height)
                    scal = imgWidth/_flightVideo.width
                else
                    scal = imgHeight/_flightVideo.height

                retPosX = (x - curWidth) * scal
                retPosY = (curHeight - y) * scal
//                if(Math.abs(retPosX) > imgWidth/2 || Math.abs(retPosY) > imgHeight/2)
//                    return;

                console.log("-----------------calPos22222", retPosX, retPosY, width*scal, height*scal)
                if(_control.type === 3 || _control.type === 4)
                    _activeVehicle.currentMount.controlPos(retPosX, retPosY, width*scal, height*scal, _control.type)
            }
        }
    }
    Timer{
        id: interval_delay
        interval: 100
        repeat: false
    }
    // 控制云台的上下左右
    Timer{
        id: control_Timer
        interval: 100
        repeat: true
        onTriggered: {
            console.log( "----------------------------------------onTriggered" );
            if(_control.type === 3 || _control.type === 4) return;
            if(control_Area.isPressed && control_Area.posiTionChanged /*&& _control.type !== 3*/)
            {
                if(Math.abs(control_Area.curPos.x - control_Area.oldPos.x) > Math.abs(control_Area.curPos.y - control_Area.oldPos.y))
                {
                    if(!_activeVehicle || _activeVehicle.mountLost || !_activeVehicle.currentMount /*|| _activeVehicle.currentMount.mountType !==0*/)
                        return; //非云台类没有此功能

                    if((control_Area.curPos.x - control_Area.oldPos.x)>0)
                        _activeVehicle.currentMount.controlHead(3, true) // move right
                    else
                        _activeVehicle.currentMount.controlHead(3, false) //move left
                }
                else{
                    if(!_activeVehicle || _activeVehicle.mountLost || !_activeVehicle.currentMount
                            || _activeVehicle.currentMount.mountType === 284 || _activeVehicle.currentMount.mountType === 362 /*|| _activeVehicle.currentMount.mountType === 1*/)
                        return; //倾斜 4G喊话 没有此功能

                    if((control_Area.curPos.y - control_Area.oldPos.y)>0)
                        _activeVehicle.currentMount.controlPitch(3, false) //down
                    else
                        _activeVehicle.currentMount.controlPitch(3, true) //up
                }
                // 放置移动后触发点的移动，作延时使用
                interval_delay.start()
            }
        }
    }
}
