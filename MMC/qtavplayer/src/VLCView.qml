/*
* VLC-Qt QML Player
* Copyright (C) 2015 Tadej Novak <tadej@tano.si>
*/

import QtQuick 2.0
import VLCQt 1.1
import QtMultimedia 5.5
import MMCSettings  1.0
import QGroundControl               1.0
import QGroundControl.Controls      1.0

Rectangle {
    property string videoSource: /*"rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov"*/MMCSettings.value("video/videoUrl","udp://@227.70.80.90:2000")

    property alias record: vidEncoder.record  //保存视频流用接口

    function screenshot(){
        vidEncoder.screenshot()
    }

    anchors.fill:   parent
    color:             Qt.rgba(0,0,0,0.75)

    onVisibleChanged: {
        if(!visible)
        {
            vidEncoder.url = ""
            vidEncoder.stop()
        }
        else{
            vidEncoder.url = videoSource
            vidEncoder.play()
        }
    }

//    VideoOutput {
//        id: videoOutput
//        source: vidPlayer
//        anchors.fill: parent
//    }


//    VlcVideoPlayer {
//        id: vidPlayer
//        anchors.fill: parent

//        decoder: vidEncoder
////        anchors.fill: parent
////        url: "udp://@127.0.0.1:1234" // "udp://@227.70.80.90:2000"
//        //"http://videos.hd-trailers.net/e23ab4b8-a737-46dd-a0e4-259ba36056b6_YsfXbEghC5XywlmkKiYrF0D1oWcPYHA94aPeKmTuUKJJVJBpH4AFbPzcNWMhAJHuRnKZAAatvKg-_8_0.mp4"
//    }

    //vidEncoder.screenshot()   //截图

    VlcVideoPlayer {
        id: vidEncoder
        url: videoSource
        anchors.fill: parent
       // record: true    //保存视频
        cache: 400      //缓存时间
        frameStep: 100   //调试用 -- 两帧相差多少ms 会打印 0代表禁用该功能
        guardTime:  parseInt( MMCSettings.value("video/guardTime", "3000"))   //守护间隔时间  -- 距离上帧刷新间隔guardTime ms 发出TriggerGuardSignal
        onVideoTrackChanged: {
//            console.log("--------------onVideoTrackChanged")
        }

        onPaintUpdataSignal: { //追加 -- 用来配合TIMER做程序自救
//            console.log("--------------onPaintUpdataSignal-------------------------------")
        }
        onTriggerGuardSignal: { //守护  --- 重启
            console.log("---------------------------------------onTriggerGuardSignal")
            vidEncoder.url = ""
            QGroundControl.msleep(100)
            vidEncoder.url = videoSource
        }
        onStateChanged: {
            console.log("--------------onStateChanged", state)
        }

    }

    //    Timer {
    //        interval : 200
    //        repeat : true
    //        running : true
    //        triggeredOnStart : true
    //        onTriggered: {
    //            vidEncoder.screenshot()
    //        }
    //    }
}








