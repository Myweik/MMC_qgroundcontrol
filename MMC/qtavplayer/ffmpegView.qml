import QtQuick 2.5
import QtQuick.Window 2.2
import qtavplayer 1.0
import QtMultimedia 5.5

//import MMCSettings  1.0
Rectangle {
    anchors.fill: parent
    color:        Qt.rgba(0,0,0,0.75)
//    property string videoSource: "rtmp://live.hkstv.hk.lxdns.com/live/hks1"   ///*"rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov" //*/  MMCSettings.value("video/videoUrl","udp://@227.70.80.90:2000") /*"udp://@227.70.80.90:2000"*/

    property bool record: false  //保存视频流用接口
    onRecordChanged: {
        avplayer.saveTs(record)
    }

    function saveImage(){ //保存图片
        avplayer.saveImage()
    }

//    onVisibleChanged: {
//        if(!visible)
//        {
//            avplayer.setUrl("")
//        }
//        else{
//            avplayer.setUrl(videoSource)
//        }
//    }

//    Text {
//        z:1
//        anchors.top: parent.top
//        anchors.right: parent.right
//        color: avplayer.encodecStatus ? "#0f0" : "#900"
//        text: avplayer.encodecStatus ? "RTMP push OK ": "RTMP push NG"
//    }

    VideoOutput {
             source: avplayer.rtspPlayer
             anchors.fill: parent
//             focus : visible // to receive focus and capture key events when visible
         }

    AVPlayer{
        id : avplayer
    }
    Component.onCompleted: {
//        if(!visible)
//        {
//            avplayer.setUrl("")
//        }
//        else{
            avplayer.setUrl(videoSource)
//        }
    }
}
