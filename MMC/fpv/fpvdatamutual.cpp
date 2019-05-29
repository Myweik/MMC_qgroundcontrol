#include "fpvdatamutual.h"
#include <QDebug>
#include <QTimer>

fpvDataMutual::fpvDataMutual(const QString &device)
: _device(device), _serial(nullptr)
{
    moveToThread(this);

    connect(this, &fpvDataMutual::sendWriteData, this, &fpvDataMutual::writeData, Qt::DirectConnection);
    _serial = new QSerialPort(this);
    connect(_serial, &QSerialPort::readyRead, this, &fpvDataMutual::readBytes, Qt::DirectConnection);

    QTimer::singleShot(10, this, SLOT(openSerial()));
    start();
}

void fpvDataMutual::run()
{
    exec();
}

void fpvDataMutual::readBytes()
{
    qDebug() << "_____________________readBytes1";
    if(!_serial)
        return;
    uint8_t tmpData[256];
    int length = _serial->read((char *)tmpData, 256);
    if(length > 0){
        QByteArray buff = QByteArray((char*)tmpData, length);
        qDebug() << "_____________________readBytes" << buff.toHex();
    }
}

int fpvDataMutual::writeData(char type, QByteArray buff)
{
    return _writeData(type, buff);
}

void fpvDataMutual::openSerial()
{
   _openSerial(_device);

   char buff[11] = {0xFF, 0x5A, 0x5B, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};
   writeData(1, QByteArray(buff, 11));
}

int fpvDataMutual::_openSerial(QString name)
{
    static int result = -1;
    if(0 == result) {
        _serial->close();
    }
    result = -1;

    _serial->setPortName(name);
    _serial->setBaudRate(QSerialPort::Baud115200);
    if(!_serial->isOpen()){
        if ( !_serial->open(QIODevice::ReadWrite)) {
            qWarning() << "Radio: Failed to open Serial Device" << name;
            return -2;
        }
    }else{
        qDebug() << "Radio: this is open Serial Device" << name;
        return -1;
    }

//    if (_serial->isOpen()) {
//        //查询一次电台 -- 通过接受判断是否为该电台 -- 相当于握手(查询ID)
//        char type = 0x8f;
//        char buff[1] = {0x01};
//        writeData(type, QByteArray(buff, 1));
//    }

    result = 0;
    return result;
}

int fpvDataMutual::_writeData(char type, QByteArray buff)
{
    return _serial->write(buff);
}
