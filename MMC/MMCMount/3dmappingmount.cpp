#include "3dmappingmount.h"
#include "QGCApplication.h"
#include "Vehicle.h"

Mapping3DMount::Mapping3DMount(Vehicle* vehicle)
        : MountInfo(vehicle)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    timer->start(LOOP_STEP);
}

void Mapping3DMount::switchControl(bool swit)
{
    uint8_t buff[5] = {0xa5, 0x03, 0x03, 0x00, 0x00};
    if(swit){
        buff[3] = 0x01;
    }else{
        buff[3] = 0x00;
    }
    swit = !swit;
    qDebug() << "--------------------------Mapping3DMount::switchControl" << swit;
    sendData(buff, buff[2] + 2);
}

void Mapping3DMount::getWifi()
{
    uint8_t buff[5] = {0xa5, 0x04, 0x03, 0x01, 0x00};
    qDebug() << "--------------------------Mapping3DMount::getWifi";
    sendData(buff, buff[2] + 2);
}

void Mapping3DMount::handleInfo(const can_data &data)
{
    // qDebug() << "----handle3DMappingInfo ";
     uint8_t tmpData[255]={0};
     for(int i=0; i < data.pack_len; i++)
     {
         if(0 < mountPacket(data.date[i], &tmpData[0]))
         {
             switch(tmpData[0])
             {
             case 1:
                     _type0x01(&tmpData[0], tmpData[1]);
                 break;
             case 2:
                     _type0x02(&tmpData[0], tmpData[1]);
                 break;
             case 3:
                     _type0x03(&tmpData[0], tmpData[1]);
                 break;
             case MOUNT_UPDATA_TYPE::MOUNT_UP_REQUESTTIME:
                 qDebug() << "----handle3DMappingInfo send time";
                 sendTime();
                 break;
             case MOUNT_UPDATA_TYPE::MOUNT_UP_REQUESTID:
                 parseMountIDAck(&tmpData[0], tmpData[1]);
                 break;
             }
         }
     }
}

void Mapping3DMount::saveJson(QJsonObject &dataJson)
{
    MountInfo::saveJson(dataJson);
    dataJson.insert("cmdcam1", cam1());                     //相机1开关状态
    dataJson.insert("cmdcam2", cam2());                     //相机2开关状态
    dataJson.insert("cmdcam3", cam3());                     //相机3开关状态
    dataJson.insert("cmdcam4", cam4());                     //相机4开关状态
    dataJson.insert("cmdcam5", cam5());                     //相机5开关状态
    dataJson.insert("cmdtfCard", tfCard());                 //TF卡在线状态
    dataJson.insert("cmdPhotoNumber", photoNumber());       //拍照数
    dataJson.insert("cmdFileName", fileName());             //POS保存文件名
    dataJson.insert("cmdWifiName", wifiName());             //wifi名
    dataJson.insert("cmdWifiPassword", wifiPassword());     //wifi密码
}

void Mapping3DMount::_type0x01(const uint8_t *ptr, int length)
{
    if(length != 7)
        return;
    parseVersion(ptr+2); //版本信息 [字节2,5]
    uint8_t tmpValue;
    memcpy(&tmpValue, ptr+6, 1); //[字节6]
    this->setCam1((tmpValue >> 0) & 0x01);
    this->setCam2((tmpValue >> 1) & 0x01);
    this->setCam3((tmpValue >> 2) & 0x01);
    this->setCam4((tmpValue >> 3) & 0x01);
    this->setCam5((tmpValue >> 4) & 0x01);
    this->setTfCard(!((tmpValue >> 5) & 0x01));
}

void Mapping3DMount::_type0x02(const uint8_t *ptr, int length)
{
    int photo_number = ( *(ptr+3) << 8) + *(ptr+2);
    int  filer_number = (int)*(ptr+4);
    this->setPhotoNumber(photo_number);
    this->setFileName("POS"+ QString::number(filer_number) + ".txt");
}

void Mapping3DMount::_type0x03(const uint8_t *ptr, int length)
{
    QString str = QString(QLatin1String((char*)(ptr + 2)));
    QStringList lst;
    lst= str.split('/',QString::SkipEmptyParts);
    QString name = lst.at(0);
    if(name.mid(1, 3) == "SID" && lst.size() >= 2){
        QStringList lstA, lstB;
        lstA = lst.at(0).split(':',QString::SkipEmptyParts);
        lstB = lst.at(1).split(':',QString::SkipEmptyParts);
        if(lstA.size() >= 2 && lstB.size() >= 2){
            this->setWifiName(lstA.at(1));
            this->setWifiPassword(lstB.at(1));
        }
    }
}

void Mapping3DMount::onTimeOut()
{
    if( ++_textNumber >> MAX_TEST_NUMBER ||(!_wifiName.isEmpty() && !_wifiPassword.isEmpty())){
        timer->stop();
        return;
    }
    getWifi();
}

