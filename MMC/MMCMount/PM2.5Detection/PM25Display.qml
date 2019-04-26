import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls.Private 1.0 as QtControlP
import MMCPMControl 1.0
import QGroundControl.Controls 1.0
import QtQuick.Window 2.2
import QGroundControl 1.0
import QtQuick.Controls 1.4
Window {//QML应用的根对象

    id:root_window
    visible: true
    minimumWidth:1000
    minimumHeight: 58
    title: qsTr("PM2.5Display")
    color: "#2F2F4F"
//   PM25Page{
//        id:pmdata;
//        onV1Changed: {
//        //    pm1_data_.text = v1;
//        }
//   }
    MouseArea{
        anchors.fill: parent
    }


    RowLayout{
        id:dataDisplay
        visible: true;
        anchors.top:parent.top;
        anchors.topMargin: 4
        anchors.left: parent.left;
        anchors.leftMargin: 12;
        spacing: 12;

        Rectangle{
            id:pm1_data;
            width: 150;
            height: 50;
            color: "black"

            Text{
                id:pm1_data_;
                color: "white"
                text:mmc_contorl_.isframe_PM1;

                anchors.centerIn: parent;
                font.pixelSize: 15

            }
        }

        Rectangle{
            id:pm25_data;
            width: 150;
            height: 50;
            color: "black"

            Text{
                color: "white";
                text:mmc_contorl_.isframe_PM25;
                anchors.centerIn: parent;
                font.pixelSize: 15;
            }
        }
        Rectangle{
            id:pm4_data;
            width: 150;
            height: 50;
            color: "black";
            Text{
                color: "white";
                text:mmc_contorl_.isframe_PM4;
                anchors.centerIn: parent;
                font.pixelSize: 15;

            }
        }
        Rectangle{
            id:pm7_data;
            width: 150;
            height: 50;
            color: "black"

            Text{
                color: "white"
                text: mmc_contorl_.isframe_PM7;
                anchors.centerIn: parent;
                font.pixelSize: 15;
            }
        }
        Rectangle{
            id:pm10_data;
            width: 150;
            height: 50;
            color: "black"

            Text{
                color: "white"
                text: mmc_contorl_.isframe_PM10;
                anchors.centerIn: parent;
                font.pixelSize: 15;
            }
        }

        Rectangle{
            id:tsp_data;
            width: 150;
            height: 50;
            color: "black"

            Text{
                color: "white"
                text: mmc_contorl_.isframe_TSP;
                anchors.centerIn: parent;
                font.pixelSize: 15;
            }
        }

    }

}



