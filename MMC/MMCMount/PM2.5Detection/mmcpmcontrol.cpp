#include "mmcpmcontrol.h"
#include <QSerialPortInfo>
#include <QDebug>
#include "qserialport.h"
#include<QList>
#include<QFile>
#include<QDateTime>
#include<QDir>
#include <QGCApplication.h>
#include "./GPS/GPSManager.h"

MMCPMControl::MMCPMControl(QObject *parent) : QObject(parent)
    , _isOpen(false)
    , _connection(false)
{
    _portList.clear();
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo portInfo, list)
    {
        _portList.append(portInfo.portName());
        emit portListChanged(_portList);
    }
//    _serial = new QSerialPort;
//    connect(_serial, SIGNAL(readyRead()), this, SLOT(readBytes())); //一收到数据就去执行readBytes函数
    _timer = new QTimer(this);
    _timer->setInterval(3000);
    _timer->setSingleShot(false);
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateSerialPorts()));
}

MMCPMControl::~MMCPMControl()
{
//    if(_serial && _serial->isOpen())
//    {
//        _serial->close();
//        _isOpen = false;
//    }
//    if(_connection)
//    {
//        if(_PM_file && _PM_file->isOpen())
//            _PM_file->close();
//    }
}

void MMCPMControl::serialPorts()
{

}

void MMCPMControl::setCoordinate(const QGeoCoordinate &coordinate)
{
    _latitude = QString::number(coordinate.latitude(),'f',8);
    _longitude = QString::number(coordinate.longitude(),'f',8);
}

QStringList MMCPMControl::serialBaudRates()
{
    QStringList tmpList;
    QList<int> list;
    list = QSerialPortInfo::standardBaudRates();
    for(int i = 0; i < list.count(); i++)
        tmpList << QString("%1").arg(list.at(i));
    return tmpList;
}

void MMCPMControl::readBytes(QByteArray buff)
{
//    QByteArray byte = _serial->readAll();  //串口读出来的是QByteArray 不是QString
//    if(qgcApp()->toolbox()->multiVehicleManager()->activeVehicle())
//    {
//        qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->sendRTKData(buff);
//    }
    qDebug() << "-------MMCPMControl::readBytes"<< buff.toHex() << buff.length();
    uint8_t rtcmData[256];
    memcpy(rtcmData, buff.data(), buff.length());
    for(int i=0; i < buff.length(); i++)
    {
        rtcm_packet(buff[i]);
    }
//    int laser_data_poi=-1;
//    laser_info += byte;
//    laser_data_poi = laser_info.indexOf("PM1");
//    if(laser_data_poi != -1&&laser_info.length() == 157)
//    {
//        QList<QString> list_PM;;
//        list_PM = QString(laser_info).split('\n', QString::SkipEmptyParts);
//        for(int i=3; i < list_PM.count(); i++)
//        {
//            QList<QString> tmpList = QString(list_PM.at(i)).replace("\r","").split(' ', QString::SkipEmptyParts);
//            if(tmpList.count() != 3)
//                return;
//            int j=0;
//            for(j=0; j < _pmList.count(); j++)
//            {
//                MMCPMItem *tmpPM = qobject_cast<MMCPMItem*> (_pmList.get(j));
//                if(tmpPM->name() == tmpList.at(0))
//                {
//                    tmpPM->setValue(tmpList.at(1));
//                    break;
//                }
//            }
//            if(j == _pmList.count())
//            {
//                MMCPMItem *item = new MMCPMItem(tmpList.at(0), tmpList.at(1), tmpList.at(2));
//                _pmList.append(item);
//            }
//        }
//        emit pmListChanged(&_pmList);
//        if(!_connection)
//        {
//            _connection = true;
//            QString dir = QString("%1").arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/MMC Station/PM2.5Report/");
//            QDir tmpDir;
//            if(!tmpDir.mkpath(dir))
//                return;
//            QString fileName = dir + QDateTime::currentDateTime().toString("yyyy_MM_dd_hhmmss")+".pm";
//            _PM_file = new QFile(fileName);
//            if(!_PM_file->open(QIODevice::WriteOnly|QIODevice::Text))
//                return;
//        }
//        if(_PM_file && _PM_file->isOpen())
//        {
//            QTextStream in(_PM_file);
//            in  << QDateTime::currentDateTime().toString("yyyy_MM_dd_hh:mm:ss")
//                << "," << _latitude
//                << "," << _longitude;
//            for(int i=0; i < _pmList.count(); i++)
//            {
//                MMCPMItem *tmpPM = qobject_cast<MMCPMItem*>(_pmList.get(i));
//                in << "," << tmpPM->name()
//                   << "," << tmpPM->value()
//                   << "," << tmpPM->unit();
//            }
//            in << "\r\n";
//        }
//        laser_info.clear();
//    }
}

void MMCPMControl::rtcm_packet(const uint8_t data)
{
//    static bool packeting = false;
    static int index = 0;
    static uint8_t pack_data[1024];

//    if(packeting)
//    {
    pack_data[index++] = data;
//    if(index > 3)
//    {
//        if(pack_data[1] != 0 || pack_data[2]> 255)
//        {
//            index=0;
//            memset(pack_data, 0, 1024);
//            packeting = false;
//        }
//    }
    if(index > 180 /*&& (pack_data[2]+6 <= (index-1))*/)
    {
        //            qDebug() << "---------------rtcm_packet"<< index << pack_data[2]+6;
        if(qgcApp()->toolbox()->multiVehicleManager()->activeVehicle())
        {
            qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->sendRTKData(QByteArray((char *)pack_data, index-1));
        }
        index=0;
        memset(pack_data, 0, 1024);
//        packeting = false;
    }
//    }
//    else if(!packeting && data == 0xD3)
//    {
//        memset(pack_data, 0, 1024);
//        packeting = true;
//        pack_data[index++] = data;
//    }
}


void MMCPMControl::updateSerialPorts()
{
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    QStringList tmpList;
    foreach (QSerialPortInfo portInfo, list) {
        tmpList.append(portInfo.portName());
    }
//    if(!_serial->isOpen())
//    {
//        _portList.clear();
//        _portList = tmpList;
//        emit portListChanged(_portList);
//    }
}
bool MMCPMControl::openPort(QString name, QString baudRate)
{
//    QSerialPortInfo info(name);

//    qgcApp()->toolbox()->gpsManager()->connectGPS(info.systemLocation());
    _serialConfig = new SerialConfiguration("RTK");
    _serialConfig->setPortName(name);
    _serialConfig->setBaud(baudRate.toInt());

    _serial = new SerialLink(_serialConfig);
    connect(_serial, &SerialLink::bytesReady, this, &MMCPMControl::readBytes);
    if(_serial->connect())
    {
        _isOpen = true;
        emit isOpenChanged(_isOpen);
        return true;
    }
//    if(_serial->open(QIODevice::ReadWrite))
//    {

//    }
    return true;
}
void MMCPMControl::closePort()
{
//    if(_serial && _serial->isOpen())
//    {
//        _serial->close();
//        _isOpen = false;
//        if(_connection)
//            if(_PM_file && _PM_file->isOpen())
//                _PM_file->close();
//        _connection = false;
//        _portList.clear();
//        emit isOpenChanged(_isOpen);
//        emit portListChanged(_portList);
//    }
    disconnect(_serial, &SerialLink::bytesReady, this, &MMCPMControl::readBytes);
    _isOpen = false;
    emit isOpenChanged(_isOpen);
    _serial->disconnect();
}


