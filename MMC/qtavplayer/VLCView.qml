/*
* VLC-Qt QML Player
* Copyright (C) 2015 Tadej Novak <tadej@tano.si>
*/

import QtQuick 2.0
import VLCQt 1.1
import QtMultimedia 5.5
//import MMCSettings  1.0
//import QGroundControl               1.0
//import QGroundControl.Controls      1.0

Rectangle {
    property string videoSource:/* "rtsp://192.168.1.129:6880/live" //*/ "rtmp://live.hkstv.hk.lxdns.com/live/hks1"// "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov" // "udp://@227.70.80.90:2000"

    property alias record: vidEncoder.record  //保存视频流用接口
    function saveImage(){ //保存图片
        vidEncoder.saveImage()
    }

    anchors.fill:   parent
    color:          Qt.rgba(0,0,0,0.75)

//    onVisibleChanged: {
//        if(!visible)
//        {
//            vidEncoder.url = ""
//        }
//        else{
//            vidEncoder.url = videoSource
//        }
//    }

    VlcVideoPlayer {
        id: vidEncoder
        url: videoSource
        anchors.fill: parent
       // record: true    //保存视频
        onStateChanged: {
            console.log("--------------onStateChanged", state)
        }
    }

    Component.onCompleted: {
        vidEncoder.url = videoSource
    }
}








