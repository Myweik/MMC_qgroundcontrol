#include "transceivermanager.h"
#include <QTimer>
#include <QGCApplication.h>

TransceiverManager::TransceiverManager(QGCApplication *app, QGCToolbox *toolbox)
    : QGCTool(app, toolbox)
{
#if defined(Q_OS_ANDROID)
    _radioMember = new AndroidRaduiMember(this);
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
    _radioMember = new RadioMember(this);
#endif
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

    _fpvMember = new FpvMember(this);
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
    _radioMember->analysisPack(type, msg);
}

void TransceiverManager::setRadioMember(RadioMemberBase *radioMember)
{
    if (radioMember && radioMember != _radioMember) {
        if (_radioMember) { delete(_radioMember); }
        _radioMember = radioMember;
        emit radioMemberChanged();
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
