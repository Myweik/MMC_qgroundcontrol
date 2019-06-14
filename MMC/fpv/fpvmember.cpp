#include "fpvmember.h"
#include <math.h>

#if defined(Q_OS_WIN32)
    #include "../../libusb0/libusb.h"
#else
    "../qtavplayer/src/usbexample.h"
#endif

FpvMember::FpvMember(QObject *parent)
    : QObject(parent)
    , _VT(0)
    , _RC(0)
{
#if defined(Q_OS_ANDROID) //"ttysWK1"
//    _fpvSerial = new fpvDataMutual("ttysWK3");
    _hidUSB = getUsbExample();
    connect(_hidUSB, &UsbExample::sendConfigData, this, &FpvMember::onReadyReadData);
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
    _hidUSB = getUsbExample();
    connect(_hidUSB, &LibUsb::sendConfigData, this, &FpvMember::onReadyReadData);
#endif
}

void FpvMember::onReadyReadData(QByteArray array)
{
//    qDebug() << "=--------------------" << array.toHex();
    char buff[4] = {0xFF, 0x5A, 0x19, 0x00};
    char buff2[4] = {0xFF, 0x5A, 0x11, 0x00};
    if(array.size() ==  (42 + 10) && array.contains(QByteArray(buff, 4))){
    uchar* buf = (uchar*)array.data() + 10;
        set_pairStatus(*(buf + 38) & 0x01);
    }else if(array.size() ==  (406 + 10) && array.contains(QByteArray(buff2, 4))){
        uchar* buf = (uchar*)array.data() + 10;

        uint16_t vt;
        memcpy(&vt, buf+2, sizeof(uint16_t));
        float vtValue =   log(vt / 64) /log(10.0) * 10;
        float vtScore = 3.52 * vtValue + 19.057;
        if(vtScore > 100)
            vtScore = 100;
        uint8_t vtErrCnt = *(buf + 375);
        if(vtErrCnt / 5){
            vtScore -= (vtErrCnt / 5) * 5;
        }else if(vtErrCnt % 5){
            vtScore -= 2;
        }
        if(vtScore <= 0){
            vtScore = 2;
        }

        uint16_t rc;
        memcpy(&rc, buf+388, sizeof(uint16_t));
        float rcValue =   log(rc / 64) /log(10.0) * 10;
        uint8_t rcLock = *(buf + 377);
        float rcScore = rcLock * (rcValue * 7.7 + 27) / 100;

        if(rcScore < 0){
            rcScore = 0;
        }
        if(rcScore > 100){
            rcScore = 100;
        }

        uint8_t lockStatus = *(buf + 359);
        if(lockStatus == 0x00){
            rcScore = 0;
            vtScore = 0;
        }
        set_VT(vtScore);
        set_RC(rcScore);
    }
}

void FpvMember::setConfig()
{
    if(_hidUSB)
        _hidUSB->setConfig();
}
