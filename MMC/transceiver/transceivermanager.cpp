#include "transceivermanager.h"
#include <QTimer>
#include <QGCApplication.h>

TransceiverManager::TransceiverManager(QGCApplication *app, QGCToolbox *toolbox)
    : QGCTool(app, toolbox)
{
    _radioMember = new RadioMember(this);
    setRadioMember(_radioMember);

#if defined(Q_OS_ANDROID)
    QTimer::singleShot(1, this, SLOT(onStartTimer()));
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
    if(MMCSettings::getvalue("app/GSType", 0).toInt() == 3)
    {
        QTimer::singleShot(1, this, SLOT(onStartTimer()));
    }
//#elif defined(Q_OS_WIN32)
//    if(MMCSettings::getvalue("app/GSType", 0).toInt() == 3)
//    {
//        QTimer::singleShot(1, this, SLOT(onStartTimer()));
//    }
#endif
}


TransceiverManager::~TransceiverManager()
{

}

void TransceiverManager::connectRadio(const QString &device)
{
    cleanup();
    _radioProvider = new RadioProvider(device);
    connect(_radioProvider, SIGNAL(RTCMDataUpdate(int, QByteArray)), this,
            SLOT(RTCMDataUpdate(int, QByteArray)));
    connect(_radioMember, SIGNAL(_writeData(char,QByteArray)), _radioProvider,
            SLOT(writeData(char,QByteArray)));
    connect(_radioProvider, &RadioProvider::closeSerialSignals, _radioProvider, &RadioProvider::closeSerialSlot,Qt::DirectConnection); //为在主线程关闭串口
    connect(_radioProvider, &RadioProvider::isconnectChanged, _radioMember, &RadioMember::setRadioLock); //在线状态

    _radioProvider->_start();

}

void TransceiverManager::setToolbox(QGCToolbox *toolbox)
{
    QGCTool::setToolbox(toolbox);

    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    qmlRegisterUncreatableType<TransceiverManager>("QGroundControl.TransceiverManager", 1, 0, "TransceiverManager", "Reference only");
    qmlRegisterUncreatableType<RadioMember>("QGroundControl.TransceiverManager",   1, 0, "RadioMember",            "Reference only");
}

void TransceiverManager::onStartTimer()
{
    //    _serialLostTimer->start();
    connectRadio("ttysWK1");
}

void TransceiverManager::RTCMDataUpdate(int type, QByteArray msg)
{
    uchar* buff = (uchar*)msg.data();
    ushort tep = 0;
    float tmp;
    if(type == 0x5f)
    qDebug() << "---------------------- TransceiverManager::RTCMDataUpdate" << type << msg.toHex();
    switch (type) {
    case 0x1f: { //遥控器各通道 -- 16字节
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channel1(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channel2(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channel3(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channel4(tep);
        buff += 2;
        for(int i = 0; i < 8; i++){
            _radioMember->setKey(i, *buff++);
        }
        break;
        }
    case 0x3f:{  //心跳
        _radioMember->set_chargeState(*buff++);
        _radioMember->setVoltage(*buff++);
        _radioMember->setQuantity(*buff++);
        _radioMember->set_time(((float)*buff++)/10);
        _radioMember->set_stateOfHealth(*buff++);
        memcpy(&tmp, buff, sizeof(float));
        _radioMember->set_temperature(tmp);
        buff += sizeof(float);
        _radioMember->set_rockerState((*buff) & 0x01);
        _radioMember->setRadioSourceState(((*buff)>>1 & 0x01));
        _radioMember->setLidState(((*buff)>>2 & 0x01));
        buff++;
        _radioMember->set_rcMode(*buff++);
        _radioMember->set_calirationState(*buff++);
        _radioMember->setVer(buff);
        buff += 4;
        break;
    }
    case 0x4f:{  //遥控器校准时各通道值
        _radioMember->set_checkStatus(*buff++);
        break;
    }
    case 0x5f:{  //遥控器校准时各通道值
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channelBMax1(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channelBMax2(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channelBMax3(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channelBMax4(tep);
        buff += 2;

        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channelBMed1(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channelBMed2(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channelBMed3(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channelBMed4(tep);
        buff += 2;

        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channelBMin1(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channelBMin2(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channelBMin3(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        _radioMember->set_channelBMin4(tep);
        buff += 2;
        break;
    }
    case 0x8f:{  //单片机唯一ID
        _radioMember->setRadioID(QByteArray((char*)buff, 12));
        break;
    }
    default:
        break;
    }
}

void TransceiverManager::setRadioMember(RadioMember *radioMember)
{
    if (radioMember && radioMember != _radioMember) {
        if (_radioMember) { delete(_radioMember); }
        _radioMember = radioMember;
        emit radioMemberChanged(_radioMember);
    }
}

void TransceiverManager::disconnectRadio()
{
    if (_radioProvider) {
        //        _requestRadioStop = true;
        //Note that we need a relatively high timeout to be sure the GPS thread finished.
        if (!_radioProvider->wait(2000)) {
            qWarning() << "Failed to wait for GPS thread exit. Consider increasing the timeout";
        }
        disconnect(_radioProvider, SIGNAL(RTCMDataUpdate(int, QByteArray)), this,
                   SLOT(RTCMDataUpdate(int, QByteArray)));
        disconnect(_radioMember, SIGNAL(_writeData(char,QByteArray)), _radioProvider,
                   SLOT(writeData(char,QByteArray)));
        delete(_radioProvider);
    }
    _radioProvider = NULL;
}

void TransceiverManager::cleanup()
{
    if (_radioProvider) {
        //        _requestRadioStop = true;
        //Note that we need a relatively high timeout to be sure the GPS thread finished.
        if (!_radioProvider->wait(1000)) {
            qWarning() << "Failed to wait for Radio thread exit. Consider increasing the timeout";
        }
        disconnect(_radioProvider, SIGNAL(RTCMDataUpdate(int, QByteArray)), this,
                   SLOT(RTCMDataUpdate(int, QByteArray)));
        disconnect(_radioMember, SIGNAL(_writeData(char,QByteArray)), _radioProvider,
                   SLOT(writeData(char,QByteArray)));
        delete(_radioProvider);
    }
    _radioProvider = NULL;
}
