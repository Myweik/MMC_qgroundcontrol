import QtQuick 2.5
import QGroundControl 1.0
import QGroundControl.Controls 1.0
import QtQuick.Layouts 1.2
import FireRec     1.0
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
Rectangle{
    id:rectangle_fire_
    implicitWidth:800
    implicitHeight: 600
//    visible: QGroundControl.multiVehicleManager.activeVehicle
    visible: true
    color: Qt.rgba(0.3,0.5,0.4,0)
    z:3
    property bool   btnsave: false
    property bool   btndraw: false
    property int    firepointlat: -1
    property int    firepointlon: -1
    property string picturepath: ""
    property var    restrict: QGroundControl.multiVehicleManager.activeVehicle ? ((QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.latitude + 90) > 90 ? 1 :(FireRec.outOfRangeAngle +0.05)) : 0
    property string savepath: ""
    //相机的等效焦距
    property var   equivalent_focal_length:/*26.8*//*22.92*/30.8
    //相机的照片的宽高比
    property var  aspect_ratio: 384/216.0
    onBtnsaveChanged: {
        if(btnsave == false)
        {
            filedialog_save_picture.open();
        }
        else
        {
            canvas_.requestPaint();
        }
    }
    FileDialog{
        id:filedialog_save_picture
        selectFolder:true
        title:"请选择文件夹"
        folder :savepath
        onAccepted: {
            var currentdate = fileUrl+"/" + Qt.formatDateTime(new Date(),"yyyyMMddhh_mm_ss")+ ".jpg";
//            console.log(currentdate);
            currentdate = currentdate.replace("file:///","") ;
//            console.log(currentdate);
            canvas_.save(currentdate);
            timer_.running = true;

        }
        onRejected: {
            timer_.running = true;
        }
    }

    Timer{
        running: QGroundControl.multiVehicleManager.activeVehicle ? !QGroundControl.multiVehicleManager.activeVehicle.gimbalLost : false
        repeat: true
        interval: 500
        onTriggered: {
            FireRec.calcu_picture_GPS(QGroundControl.multiVehicleManager.activeVehicle.latitude,QGroundControl.multiVehicleManager.activeVehicle.longitude,(QGroundControl.multiVehicleManager.activeVehicle.altitudeRelative.rawValue/1000),
           'N','E',equivalent_focal_length,QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.latitude,
          QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.longitude + QGroundControl.multiVehicleManager.activeVehicle.heading.rawValue,QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.altitude,aspect_ratio);
        }
    }
    Timer{
        id:timer_
        running:false
        repeat:false
        interval: 500
        onTriggered: {
            canvas_.requestPaint();
        }
    }
    Image {
        id: imagefire
        source: "/qmlimages/fire.svg"
        visible: false
        width: 30
        height: 50
        z:2
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: !FireRec.calcufinish
        visible: !FireRec.calcufinish
//        onRunningChanged: {
//            console.log("-------onRunningChanged",FireRec.calcufinish);
//        }
    }
    Rectangle{
        color: Qt.rgba(1,1,1,0.6)
        z:3
        width:text_gimbal_.width +10
        height:text_gimbal_.height + 10
        anchors.right:  parent.right
        anchors.top:    parent.top
        anchors.topMargin:  20
        anchors.rightMargin:20

        Text {
            id: text_gimbal_
            anchors.centerIn: parent
            font.pointSize: 20
            text:  QGroundControl.multiVehicleManager.activeVehicle ? ((QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.latitude + 90) > 0 ? (QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.latitude + 90).toFixed(2) : 0) : ""
        }
        MouseArea{
            id:mousearea_hide
            property bool firehide: false
            anchors.fill: parent
            onClicked: {
                timer_hide.running = true;
                mousearea_hide.enabled = false;
                if(firehide == false)
                {
                    firehide = true;
                    canvas_.visible = false;
                    rectangle_repeater.visible = false;
                    repeater_.selectnum = 5;
                    mousearea_fire_.enabled = false;
                    rectangle_restrict.visible = false;
                }
                else
                {
                    firehide = false;
                    canvas_.visible = true;
                    mousearea_fire_.enabled = true;
                    repeater_.selectnum = 5;
                    rectangle_repeater.visible = true;
                    rectangle_restrict.visible = true;

                }
            }
        }
        Timer{
            id:timer_hide
            running: false
            repeat: false
            interval: 100
            onTriggered: {
                mousearea_hide.enabled = true;
            }
        }
    }


//    Text {
//        id: text_yaw
//        anchors.right:  parent.right
//        anchors.top:    text_gimbal_.bottom
//        anchors.topMargin:  20
//        anchors.rightMargin:20
//        font.pointSize: 20
//        text:(QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.longitude + QGroundControl.multiVehicleManager.activeVehicle.heading.rawValue).toFixed(0)
//    }
    property double isrectx : 0
    property double isrecty : 0
    property var list: []

    Rectangle{
        id:rectangle_restrict
        width: parent.width
        height: restrict < 0 ? 0 : restrict * parent.height
        color: Qt.rgba(0.242,0.256,0.177,0.5)
        Rectangle{
            width: parent.width
            height: 2
            anchors.bottom: parent.bottom
            color: "red"
        }
    }

    Canvas{
        id:canvas_
        anchors.fill: parent
        visible: true
        property bool       fillpath: false
        property variant    image;
        property bool       drawfirepoint: false
        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(canvas_.x,canvas_.y,canvas_.x + canvas_.width,canvas_.y + canvas_.height);
            if(btndraw == true)
            {
                ctx.drawImage(picturepath,0,0,canvas_.width,canvas_.height);
                ctx.stroke();
//                console.log("------------drawimage");

            }
//            console.log("------------on paint");
            ctx.lineWidth = 5;
            ctx.strokeStyle = "red";
            ctx.fillStyle = Qt.rgba(0,0.5,0.5,0.5);
            if(list.length >= 1 )
            {
                ctx.beginPath();
                ctx.moveTo(list[0].x / 1600 * parent.width,list[0].y / 900 * parent.height);
                for(var i = 1;i < list.length;i++)
                {
                    ctx.lineTo(list[i].x / 1600 * parent.width,list[i].y / 900 * parent.height);
                }
                ctx.stroke();
            }
            if(fillpath && (list.length || imagefire.visible))
            {
//                console.log("-----------------fillpath");
                if(list.length > 2)
                {
                    ctx.closePath();
                    ctx.fill();
                    ctx.stroke();
                }

                ctx.strokeStyle = Qt.rgba(0.9,0.9,0.9,0.8);
                ctx.fillStyle = Qt.rgba(0.9,0.9,0.9,0.8);
                ctx.beginPath();
                if(FireRec.firepoint == true && imagefire.visible == true)
                {
                    firepointlat = FireRec.qfirepoint[0].latitude.toFixed(6);
                    firepointlon = FireRec.qfirepoint[0].longitude.toFixed(6);
                }
                if(firepointlat != -1 && firepointlon != -1 && imagefire.visible == true && list.length >= 1)
                {
                    ctx.rect(0,0,280,160);
                    ctx.fill();
                    ctx.stroke();
                }
                else if(list.length >= 1)
                {
                    ctx.rect(0,0,280,90);
                    ctx.fill();
                    ctx.stroke();
                }
                else if(firepointlat != -1 && firepointlon != -1 && imagefire.visible == true && list.length == 0)
                {
                    ctx.rect(0,0,280,90);
                    ctx.fill();
                    ctx.stroke();
                }

                ctx.strokeStyle = "black";
                ctx.lineWidth = 1;
                ctx.fillStyle = "black"
                ctx.beginPath();
                ctx.font = "20px sans-serif"
                if(firepointlat != -1 && firepointlon != -1 && imagefire.visible == true)
                {
                    if(list.length)
                    {
                        ctx.text("纬度 : " + firepointlat,20,110);
                        ctx.text("经度 : " + firepointlon,20,150);
                    }
                    else
                    {
                        ctx.text("纬度 : " + firepointlat,20,30);
                        ctx.text("经度 : " + firepointlon,20,70);
                    }
                }
                if(list.length >= 1)
                {
                    ctx.text("面积：" + FireRec.sumArea.toFixed(1) + " m^2",20,30);
                    ctx.text("线长：" + FireRec.lineLength.toFixed(1) + " m",20,70);
                    if(FireRec.pointmode)
                    {
                        for(var j = 0;j < list.length - 1;j++)
                        {
                            ctx.text( FireRec.linesectionlength[j].toFixed(1) + "m",(list[j].x + list[j + 1].x) / 2 / 1600 * parent.width,(list[j].y + list[j + 1].y) / 2 / 900 * parent.height);
                        }
                        if(list.length > 2)
                        {
                            ctx.text( FireRec.linesectionlength[list.length - 1].toFixed(1) + "m",(list[0].x + list[list.length - 1].x) / 2 / 1600 * parent.width,(list[0].y + list[list.length - 1].y) / 2 / 900 * parent.height);
                        }
                    }





                }
                ctx.fill();
                ctx.stroke();
            }

            if(btndraw == true && imagefire.visible == true)
            {
                ctx.drawImage(imagefire.source,imagefire.x,imagefire.y,30,50);
//                ctx.strokeStyle = Qt.rgba(0.9,0.9,0.9,0.8);
//                ctx.fillStyle = Qt.rgba(0.9,0.9,0.9,0.8);
//                ctx.beginPath();
//                ctx.rect(canvas_.x + canvas_.width - 100,canvas_.y + canvas_.y + canvas_.height - 50 ,100,50);
//                ctx.fill();
//                ctx.stroke();

            }

            if(btnsave == true)
            {
                ctx.strokeStyle = "black";
                ctx.fillStyle = "black"
                ctx.lineWidth = 1;
                ctx.font = "15px sans-serif"
                ctx.beginPath();
                ctx.text(Qt.formatDateTime(new Date(),"yyyy-MM-dd hh:mm"),canvas_.x + canvas_.width - 120,canvas_.y + canvas_.y + canvas_.height - 20)
                ctx.fill();
                ctx.stroke();
                btnsave = false;
                btndraw = false;
            }

        }

    }
    Rectangle{
        id:rectangle_repeater
        color: Qt.rgba(0,0,0,0.7)
        width: image_rowLayout.width + 20
        height: image_rowLayout.height + 10
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 10
        visible: true
        z:2
        MouseArea{
            anchors.fill: parent
        }

        RowLayout{
            id:image_rowLayout
            anchors.centerIn: parent
//            height: parent.height
//            width: repeater_.width + 10
            spacing: 40
            Repeater{
                id:repeater_

                model: ["/qmlimages/fireline.svg","/qmlimages/firepoint.svg","/qmlimages/fire.svg","/qmlimages/firesave.svg","/qmlimages/fireok.svg","/qmlimages/fireX.svg"]
                property int selectnum: 100
                property int lastnum: 100
                Rectangle{

                    width: 50
                    height:50
                    color:  Qt.rgba(0,0,0,0)
                    border.color: "white"
                    visible: index == 0 ? false : true
                    border.width: repeater_.selectnum == index ? 3 : 0
                    anchors.verticalCenter: parent.verticalCenter
                    Image {
                        anchors.centerIn: parent
                        source: modelData
                        z:2
                        sourceSize: Qt.size(70,30)

                    }
                    MouseArea{
                        enabled: FireRec.calcufinish
                        anchors.fill: parent
                        onClicked: {
                            repeater_.selectnum = index;
                        }
                    }
                }
                onSelectnumChanged: {
                    switch(selectnum)
                    {
                    case 0:
                        canvas_.fillpath = false;
                        mousearea_fire_.enabled = true;
                        rectangle_pos_.isVisible = false;
                        list = [];
                        FireRec.clearGpsStore();
                        canvas_.requestPaint();
                        FireRec.pointmode = false;
                        FireRec.firepoint = false;
                        lastnum = selectnum;
                        if(FireRec.drawFinish == true)
                            FireRec.drawFinish = false;
                        break;
                    case 1:
                        list = [];
                        FireRec.clearGpsStore();
                        canvas_.requestPaint();
                        canvas_.fillpath = false;
                        mousearea_fire_.enabled = true;

                        canvas_.requestPaint();
                        FireRec.pointmode = true;
                        FireRec.firepoint = false;
                        lastnum = selectnum;
                        if(FireRec.drawFinish == true)
                            FireRec.drawFinish = false;
                        break;
                    case 2:
                        mousearea_fire_.enabled = true;
//                        list = [];
//                        FireRec.clearGpsStore();
                        canvas_.requestPaint();
                        FireRec.pointmode = false;
                        FireRec.firepoint = true;

                        lastnum = selectnum;
                        break;
                    case 3:
                        savepath = FireRec.isDirExist();
                        btndraw = true;
                        selectnum = lastnum;
                        break;
                    case 4:
                        if(lastnum == 5)
                        {
                            return;
                        }

                        mousearea_fire_.enabled = false;
                        canvas_.fillpath = true;
                        FireRec.drawFinish = true;
                        FireRec.calcuArea();
                        FireRec.calculine();
                        lastnum = selectnum;
                        canvas_.requestPaint();
                        break;
                    case 5:
                        list = [];
                        FireRec.clearGpsStore();

                        rectangle_pos_.isVisible = false;
                        imagefire.visible = false;
                        mousearea_fire_.enabled = false;
                        canvas_.requestPaint();
                        selectnum = lastnum;

                        break;
                    }
                }
            }
        }
    }

    // 点击消抖
    Timer{
        id:timer_mousearea
        interval: 100
        repeat: false
        running: false
        onTriggered: {
            mousearea_fire_.enabled = true;
        }
    }

    MouseArea{
        id:mousearea_fire_
        property bool isPressed: false
//        anchors.top: restrict = -1 ? parent.top : (parent.top + parent.height * restrict)
//        width:parent.width
//        anchors.bottom: parent.bottom
        anchors.fill:parent
        enabled: false
        onClicked: {
            if(repeater_.selectnum == 1)
                rectangle_pos_.isVisible = true;
            else if(repeater_.selectnum == 2)
            {
                rectangle_pos_.isVisible = false;

//                FireRec.clearGpsStore();
//                list = [];

                parent.isrectx = mouseX / parent.width;
                parent.isrecty = mouseY / parent.height;
                imagefire.x = parent.isrectx * parent.width - imagefire.width / 2;
                imagefire.y = parent.isrecty * parent.height - imagefire.height;
                if(parent.isrecty < restrict)
                {
                    imagefire.visible =false;
                    return;
                }
                imagefire.visible = true;

                FireRec.calcu_picture_GPS(QGroundControl.multiVehicleManager.activeVehicle.latitude,QGroundControl.multiVehicleManager.activeVehicle.longitude,(QGroundControl.multiVehicleManager.activeVehicle.altitudeRelative.rawValue/1000),
                                           'N','E',equivalent_focal_length,QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.latitude,
                                          QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.longitude + QGroundControl.multiVehicleManager.activeVehicle.heading.rawValue,QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.altitude,aspect_ratio);


//                list.push(Qt.point(parent.isrectx * 1600,parent.isrecty * 900));
                FireRec.square(parent.isrectx * 1600,parent.isrecty * 900);
            }


            if(rectangle_pos_.isVisible == true)
            {

                parent.isrectx = mouseX / parent.width;
                parent.isrecty = mouseY / parent.height;
                if(parent.isrecty < restrict)
                {
                    return;
                }
                FireRec.calcu_picture_GPS(QGroundControl.multiVehicleManager.activeVehicle.latitude,QGroundControl.multiVehicleManager.activeVehicle.longitude,(QGroundControl.multiVehicleManager.activeVehicle.altitudeRelative.rawValue/1000),
                                           'N','E',equivalent_focal_length,QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.latitude,
                                          QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.longitude + QGroundControl.multiVehicleManager.activeVehicle.heading.rawValue,QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.altitude,aspect_ratio);


                list.push(Qt.point(parent.isrectx * 1600,parent.isrecty * 900));
                canvas_.requestPaint();
                FireRec.square(parent.isrectx * 1600,parent.isrecty * 900,1);


            }
        }
        onPressed: {
            if(repeater_.selectnum !=0)
            {
                return;
            }
            isPressed = true
            parent.isrectx = mouseX / parent.width;
            parent.isrecty = mouseY / parent.height;
        }
        onReleased: {
            isPressed = false;
            timer_mousearea.start();
            mousearea_fire_.enabled = false;
            if(repeater_.selectnum !=0)
            {
                return;
            }
            mousearea_fire_.focus = false;
            FireRec.calcu_picture_GPS(QGroundControl.multiVehicleManager.activeVehicle.latitude,QGroundControl.multiVehicleManager.activeVehicle.longitude,(QGroundControl.multiVehicleManager.activeVehicle.altitudeRelative.rawValue/1000),
                                       'N','E',equivalent_focal_length,QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.latitude ,
                                      QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.longitude + QGroundControl.multiVehicleManager.activeVehicle.heading.rawValue,QGroundControl.multiVehicleManager.activeVehicle.gimbalGesture.altitude,aspect_ratio);

            for(var i = 0;i < list.length;i++)
            {
                FireRec.square(list[i].x,list[i].y,list.length);
            }

        }
        onPositionChanged: {
            if(repeater_.selectnum !=0 && !isPressed)
                return;

            parent.isrectx = mouseX / parent.width;
            parent.isrecty = mouseY / parent.height;
            if(parent.isrecty < restrict)
            {
                return;
            }

            canvas_.requestPaint();
            if(mouseX >= 0 && mouseY >= 0)
                list.push(Qt.point(parent.isrectx * 1600,parent.isrecty * 900));
        }

    }
    Rectangle{
        id:rectangle_pos_

        property bool isLast: true
        property bool isVisible: false
        property int maxx: rectangle_fire_.x + rectangle_fire_.width -rectangle_pos_.width
        property int maxy: rectangle_fire_.y + rectangle_fire_.height - rectangle_pos_.height
        property int minx: rectangle_fire_.x
        property int miny: rectangle_fire_.y
        property bool isEnter: false
        property bool isExit: false
        property bool isRelease: false
        x:parent.isrectx * parent.width
        y:parent.isrecty * parent.height
        width: 10
        height:10
        color: isLast ? "orange" : "#0000FF"
        visible:  isVisible
        MouseArea{
           anchors.fill: parent
           drag.target: rectangle_pos_
           hoverEnabled: true
           drag.maximumX: parent.maxx
           drag.maximumY: parent.maxy
           drag.minimumX: parent.minx
           drag.minimumY: parent.miny
           onEntered: {
               parent.color = "lightgreen";
               parent.isEnter = true;
           }
           onExited: {
               parent.color = parent.isLast ? "orange" : "#0000FF";
               parent.isExit = true;
           }
           onReleased: {
               parent.isRelease = true;
           }
        }
    }
}

