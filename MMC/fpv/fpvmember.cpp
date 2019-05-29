#include "fpvmember.h"

FpvMember::FpvMember(QObject *parent) : QObject(parent)
{
#if defined(Q_OS_ANDROID) //"ttysWK1"
//    _fpvSerial = new fpvDataMutual("ttysWK3");
    _hidUSB = getUsbExample();
    connect(_hidUSB, &UsbExample::sendConfigData, this, &FpvMember::onReadyReadData);
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN32)

#endif

}

void FpvMember::onReadyReadData(QByteArray array)
{

    char buff[4] = {0xFF, 0x5A, 0x19, 0x00};
    if(array.size() ==  52 && array.contains(QByteArray(buff, 4))){
    uchar* buf = (uchar*)array.data() + 10;
        set_pairStatus(*(buf + 38) & 0x01);
//        qDebug() << "----------------------------onConfigReadyRead" << array.toHex() << *(buf + 38);
    }
}

void FpvMember::setConfig()
{
    if(_hidUSB)
        _hidUSB->setConfig();
}
