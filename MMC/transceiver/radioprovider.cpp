#include "radioprovider.h"
#include "QGCApplication.h"

#include "QGCSerialPortInfo.h"

#include "MMC/mmcplugin.h"

RadioProvider::RadioProvider(const QString &device)
    : _device(device), _serial(nullptr)
{
    moveToThread(this);

    _openSerialTimer = new QTimer(this);
    _openSerialTimer->setInterval(250);
    connect(_openSerialTimer, SIGNAL(timeout()), this, SLOT(openSerialSlot()),Qt::QueuedConnection);

    _serial = new QSerialPort();
    connect(_serial, &QSerialPort::readyRead, this, &RadioProvider::readBytes, Qt::DirectConnection);
    start();
}

RadioProvider::~RadioProvider()
{
    quit();
    wait();

    if (_serial->isOpen()) {
        _serial->close();
        QObject::disconnect(_serial, &QSerialPort::readyRead, this, &RadioProvider::readBytes);
    }
    delete(_serial);
    _serial = nullptr;
}

void RadioProvider::_start()
{
    qDebug() << "--------------------------RadioProvider::_start()";
    _openSerialTimer->start();
}

int RadioProvider::writeData(char type, QByteArray buff)
{
    return _writeData(type, buff);
}

int RadioProvider::openSerial(QString name)
{
    qDebug() << "---------------------openSerial" << name;
    static int result = -1;
    if(0 == result) {
        emit closeSerialSignals();
        msleep(10);
        //        _serial->close();
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

#if !defined(Q_OS_ANDROID)
    if (_serial->isOpen()) {
        //查询一次电台 -- 通过接受判断是否为该电台 -- 相当于握手(查询ID)
        char type = 0x8f;
        char buff[1] = {0x01};
        writeData(type, QByteArray(buff, 1));
    }
#endif

    result = 0;
    return result;
}

void RadioProvider::openSerialSlot()
{
    //    qDebug() << "------------------------openSerialSlot" << GetCurrentThreadId();
    if (_isconnect <= 0){ //没上线
#if defined(Q_OS_ANDROID)
    openSerial("ttysWK1");
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
        static QList<QGCSerialPortInfo> portList = QGCSerialPortInfo::availablePorts();
        static int i = 0;
        if(i == portList.count()){
            portList = QGCSerialPortInfo::availablePorts();
            i = 0;
        }
        if(i == 0) {  //缩小串口范围
            QList<QGCSerialPortInfo> portListTmp = portList;            portList.clear();
            for(int j = 0; j < portListTmp.count(); j++){
                if(portListTmp.at(j).description().contains("CP210x") /*&& portListTmp.at(j).isBusy()*/)
                    portList.append(portListTmp.at(j));
            }
            if(portList.count() == 0) msleep(5000); //休息5S
        }
        if(i < portList.count()){
            openSerial(portList.at(i++).portName());
        }
#endif
    }else{
#if !defined(Q_OS_ANDROID)
        if(--_isconnect <= 0)
            emit isconnectChanged(false); //掉线
#endif
    }
}

void RadioProvider::closeSerialSlot()
{
    if(_serial && _serial->isOpen())
        _serial->close();
}

void RadioProvider::run()
{
    exec();
}

void RadioProvider::readBytes()
{
    if(!_serial)
        return;
    uint8_t tmpData[256];
    int length = _serial->read((char *)tmpData, 256);
    if(length > 0){
        QByteArray buff = QByteArray((char*)tmpData, length);
        analysisPack(buff);
    }
}

void RadioProvider::analysisPack(QByteArray buff)
{
//    qDebug() << "-------------buff" << buff.toHex();
    uint8_t tmpData[256];
    memset(tmpData, 0x00, 256 * sizeof(uint8_t));
    for(int i = 0; i < buff.length(); i++){
        if(0 < splicePacket((buff.at(i)), tmpData)){
            emit RTCMDataUpdate(tmpData[1], QByteArray((char*)&tmpData[3], tmpData[2]));

            if(_isconnect <= 0){
                emit isconnectChanged(true);
            }
            _isconnect = 20; //重置标志位
        }
    }
}

int RadioProvider::splicePacket(const uint8_t data, uint8_t *dst)
{
    static bool packeting = false;
    static QList<uint8_t> packList;

    // 5A 5A  index type len data check
    if(packeting)
    {
        if(packList.count() == 1 && data != 0xA5){ //判断第二个字节
            packeting = false;
            return -1;
        }

        packList.append(data);
        if(packList.count() < 6) return -1;

        if(packList.count() > 6 && packList.at(4)+6 <= packList.count())
        {
            packeting = false;
            uint8_t tmpData[255]={0};
            for(int i=0; i < packList.at(4) + 4; i++)
                tmpData[i] = packList.at(i+2);
//#if defined(Q_OS_ANDROID)
//            uint8_t check = MMC::_xor8(&tmpData[3], tmpData[2]);
//#else
            uint8_t check = MMC::_crc8(&tmpData[1], tmpData[2] + 2);
//#endif
//            qDebug() << "---------------------------crc" << check << tmpData[tmpData[2] + 3];
            if(check != tmpData[tmpData[2] + 3]) //crc
                return -1;
            memcpy(dst, &tmpData[0], (size_t)(tmpData[2] + 4));
            return tmpData[2] + 4;
        }
    }
    else if(!packeting && data == 0x5A)
    {
        packList.clear();
        packeting = true;
        packList.append(data);
    }
    return -1;
}

// 5A 5A  index type len data check
int RadioProvider::_writeData(char type, QByteArray buff)
{
    static uchar index = 0;
    uint8_t len = buff.length();
    uint8_t fileData[256] = {0};
    uint8_t* buf = fileData;
    *buf++ = 0x5A;   //--头
    *buf++ = 0xA5;   //--id
    *buf++ = index++;//--index
    *buf++ = type;   //--type
    *buf++ = len;   //--len
    memcpy(buf, buff.data(), len); //data
    buf += len;
    *buf++ = MMC::_crc8((uchar*)&fileData[3], len + 2);   //--check - CRC
    qDebug() << "------------------RadioProvider::_writeData" << len  << QByteArray((char*)fileData, len + 6).toHex();
    return _serial->write((char*)fileData, len + 6);  //zhi neng zai zhe fa song
}

