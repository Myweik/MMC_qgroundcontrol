#include "mountinfo.h"
#include "QGCApplication.h"
#include <QDebug>

MountInfo::MountInfo(Vehicle* vehicle, QObject* parent) : QObject(parent)
  , _vehicle(vehicle)
  ,_mountId(0)
  , _version("")
  , _hasMountId(false)
{
    //上下行id 对应
    _canId[266] = 10; //喊话器
    _canId[362] = 106; //4G喊话器
    _canId[282] = 26; //探照灯
    _canId[298] = 42; //抛投
    _canId[346] = 90; //毒气检测
    _canId[267] = 11; //10倍云台
    _canId[283] = 27; //20倍松下云台
    _canId[523] = 11; //高清双光
    _canId[299] = 43; //20倍索尼云台
    _canId[315] = 59; //30倍奇蛙云台
    _canId[331] = 75; //Gopro5云台
    _canId[347] = 91; //10倍红外双光云台
    _canId[363] = 107; //小双光云台
    _canId[379] = 123; //18倍微光
    _canId[395] = 139; //35倍微光带跟踪
    _canId[411] = 155; //5100云台
    _canId[427] = 171; //A7R云台
    _canId[284] = 28; //倾斜摄影
    _canId[269] = 12; //飞控指示灯
    _canId[300] = 44; //氢燃料
    _canId[314] = 58; //催泪弹
    _canId[443] = 187;//filr红外
    _canId[491] = 235;//海视英科红外

    //上行id 对应型号
    _idString[266] = "P1"; //喊话器
    _idString[362] = "PR180"; //4G喊话器
    _idString[282] = "L30"; //探照灯
    _idString[523] = "ZT40"; //高清双光
    _idString[298] = "S1"; //抛投
    _idString[346] = "PES1"; //毒气检测
    _idString[267] = "PGVT1Z10"; //10倍云台
    _idString[283] = "PGVS1Z20"; //20倍松下云台
    _idString[299] = "PGVN4Z20"; //20倍索尼云台
    _idString[315] = "PGVQ1Z30"; //30倍奇蛙云台
    _idString[331] = "PGVG5"; //Gopro5云台
    _idString[347] = "PG2IF1"; //10倍红外双光云台
    _idString[363] = "PG2IF1LT2Z35"; //小双光云台
    _idString[379] = "PGLH1Z18"; //18倍微光
    _idString[395] = "PGLT2Z35"; //35倍微光带跟踪
    _idString[411] = "PGVN5Z02"; //5100云台
    _idString[427] = "PGVN7Z02"; //A7R云台
    _idString[443] = "PGIF1";
    _idString[475] = "PG2IF1-LT2Z35";//F1-T2系35倍双光云台
    _idString[491] = "PGIY1";//海视英科红外
    _idString[507] = "PG2IF2_LT2Z35";//微光红外二合一长款

    _idString[284] = "PO5N2A1530B"; //倾斜摄影
    _idString[269] = ""; //飞控指示灯
    _idString[300] = ""; //氢燃料
    _idString[314] = ""; //催泪弹

    //上行id 对应名字3
    _idName[266] = tr("Speaker", "轰天雷P1"); //喊话器
    _idName[362] = tr("4GSpeaker", "轰天雷P2"); //4G喊话器
    _idName[282] = tr("Light", "独火星"); //探照灯
    _idName[298] = tr("Intelligent","及时雨"); //抛投
    _idName[346] = tr("Gas Detection", "白日鼠"); //毒气检测
    _idName[267] = tr("Gimbal","云台"); //10倍云台
    _idName[283] = tr("Gimbal","云台"); //20倍松下云台
    _idName[523] = tr("Gimbal","云台"); //高清双光
    _idName[299] = tr("Gimbal","云台"); //20倍索尼云台
    _idName[315] = tr("Gimbal","云台"); //30倍奇蛙云台
    _idName[331] = tr("Gimbal","云台"); //Gopro5云台
    _idName[347] = tr("Gimbal","云台"); //10倍红外双光云台
    _idName[363] = tr("Gimbal","云台"); //小双光云台
    _idName[379] = tr("Gimbal","云台"); //18倍微光
    _idName[395] = tr("Gimbal","云台"); //35倍微光带跟踪
    _idName[411] = tr("Gimbal","云台"); //5100云台
    _idName[427] = tr("Gimbal","云台"); //A7R云台
    _idName[491] = tr("Gimbal","云台");//海视英科红外
    _idName[443] = tr("Infra-red","热成像T"); //Filr红外
    _idName[284] = tr("Oblique Photography","金眼彪M5"); //倾斜摄影
    _idName[269] = tr(""); //飞控指示灯
    _idName[300] = tr(""); //氢燃料
    _idName[314] = tr("TearBomb", "催泪弹"); //催泪弹
}

void MountInfo::setVersion(QString version)
{
    _version = version;
    emit versionChanged();

    if(!_hasMountId)
        requestMountID();
}

void MountInfo::parseMountIDAck(const uint8_t *ptr, int length)
{
    if(length != 26)
            return;
    uint16_t manufacturer, type; //供应商 - 挂载类型
    uint32_t mountID[4];
    memcpy(&manufacturer, ptr+2, 2);
    memcpy(&type, ptr+4, 2);
    memcpy(mountID, ptr+6, 16);
    //        memcpy(&version, ptr+22, 4);
    setMountID(mountID); //ID
    parseVersion(ptr+22); //版本号
        //      qDebug() << "-------------Vehicle::parseMountIDAck" << manufacturer << type << mountID[0] << mountID[1] << mountID[2] << mountID[3]<< version;
}

void MountInfo::sendTime()
{
    uint8_t buff[10]={0xA5,0x13,8,0,0,0,0,0,0,0};
    int16_t year = QDateTime::currentDateTime().date().year();
    memcpy(&buff[3], &year, 2);
    buff[5] = QDateTime::currentDateTime().date().month();
    buff[6] = QDateTime::currentDateTime().date().day();
    buff[7] = QDateTime::currentDateTime().time().hour();
    buff[8] = QDateTime::currentDateTime().time().minute();
    qDebug() << "--------------------------MountInfo::sendTime";
    sendData(buff, 10);
}

void MountInfo::controlCommand(QString code)
{
    QString str = code.remove(QRegExp("\\ "));
    uint8_t tmpBuffer[200];
    tmpBuffer[0] = 0xa5;
    tmpBuffer[1] = 0x0f;
    tmpBuffer[2] = str.length() / 2 + 2;

    QByteArray tmpStr = str.toLatin1();
    for(int j=0; j < tmpStr.length()/2; j++)
    {
        bool ok;
        tmpBuffer[j+3] = tmpStr.mid(j*2, 2).toInt(&ok, 16);
//        qDebug() << j << tmpBuffer[j+3];
    }
    qDebug() << "--------------------------MountInfo::controlCommand " << QString::fromLocal8Bit("透传") << str;
    sendData(tmpBuffer, str.length() / 2 + 4);
}

void MountInfo::requestMountID()
{
    if(_hasMountId)
        return;
    uint8_t buff[5]={0xA5,0xFF,0x03,0x01,0x00};
    qDebug() << " -----------------requestMountID";
    sendData(buff, 5);
}

void MountInfo::setMountID(uint32_t *mountId)
{
    _hasMountId = true;
    memcpy(_mountRequestId, mountId, 16);
    _mountID = QString("%1").arg(_mountRequestId[0], 8, 16, QLatin1Char('0')) + QString("%1").arg(_mountRequestId[1], 8, 16, QLatin1Char('0'))
            + QString("%1").arg(_mountRequestId[2], 8, 16, QLatin1Char('0')) + QString("%1").arg(_mountRequestId[3], 8, 16, QLatin1Char('0'));
    qDebug() << "--------setMountID" << _mountID;
    //    _mountRequestId = mountId;
    emit mountIDChanged();
}

void MountInfo::saveJson(QJsonObject &dataJson)
{
    dataJson.insert("cmdJsonType", "Mount");        //json类型
    dataJson.insert("cmdJsonVersion", 1.0);         //json版本号
    dataJson.insert("cmdCanId", readId());          //挂载上行ID
    dataJson.insert("cmdMountId", mountID());       //挂载唯一ID
    dataJson.insert("cmdMountVersion", version());  //挂载版本号
    dataJson.insert("cmdMountType", mountType());   //挂载类型 -- 0-代表云太类 1-代表未识别的通用挂载
}

/* *********************************************************************
 * :解析数据
 * ********************************************************************/
void MountInfo::parseVersion(const uint8_t *p)
{
    uint32_t version;
    memcpy(&version, p, 4);
    int a = version &0x1f; //日
    int b = (version >> 5) & (0x0f);//月
    int c = (version >> (5+4)) & (0x7f);//年
    int d = (version >> (5+4+7)) & (0xff);//优化
    int e = (version >> (5+4+7+8)) & (0x3f);//功能增删
    int f = (version >> (5+4+7+8+6)) & (0x03);//硬件平台
    QString year;
    if(c < 10)
        year = QString("0%1").arg(c);
    else
        year = QString("%1").arg(c);
    QString mouth;
    if(b < 10)
        mouth = QString("0%1").arg(b);
    else
        mouth = QString("%1").arg(b);
    QString day;
    if(a < 10)
        day = QString("0%1").arg(a);
    else
        day = QString("%1").arg(a);
    QString camVersion = QString("%1.%2.%3.%4%5%6").arg(f).arg(e).arg(d).arg(year).arg(mouth).arg(day);
    setVersion(camVersion);
}

/* 解析基本函数 */
int MountInfo::mountPacket(const uint8_t data, uint8_t *dst)
{
    if(_packeting)
    {
        _packList.append(data);
        if(_packList.count() > 2 && _packList.at(2)+2 <= _packList.count())
        {
            _packeting = false;
            uint8_t tmpData[255]={0};
            for(int i=1; i < _packList.at(2) + 1; i++)
                tmpData[i-1] = _packList.at(i);
            uint8_t crc = QGC::mount_crc(&tmpData[0], tmpData[1]);


            if(crc != _packList.at(_packList.count() - 1))
                return -1;
            memcpy(dst, &tmpData[0], (size_t)tmpData[1]);
           // qDebug() << "-----mount_packet" << tmpData[1];
            return tmpData[1];
        }
    }
    else if(!_packeting && data == 0xA5)
    {
        _packList.clear();
        _packeting = true;
        _packList.append(data);
    }
    return -1;
}

void MountInfo::sendData(uint8_t *buf, int len)
{
    uint8_t *buff = buf;
    mavlink_message_t msg;
    mavlink_mavlink_2_can_t can;

    can.can_id = writeId();
    *(buff + len - 1) = QGC::mount_crc(buff+1, len-2);  //crc
    can.valid_len = len;
    memcpy(can.buf, buf, len);
    mavlink_msg_mavlink_2_can_encode(qgcApp()->toolbox()->mavlinkProtocol()->getSystemId(),
                                     qgcApp()->toolbox()->mavlinkProtocol()->getComponentId(),
                                     &msg,
                                     &can);

    qDebug() << QString("----------------sendData id:%1 len:%6 data:%7" )
                .arg(can.can_id).arg(len).arg(QString(QByteArray((char*)buf, len).toHex()));

    _vehicle->sendMessageOnLink(_vehicle->priorityLink(), msg);
}
