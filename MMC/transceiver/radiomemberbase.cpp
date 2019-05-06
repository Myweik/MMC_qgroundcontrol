#include "radiomemberbase.h"
#include <QDebug>

#define  R_MODE  0X0A
#define  L_MODE  0X05


MMCKey::MMCKey(int id, QObject *parent) : QObject(parent), _id(id)
{
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));

}

void MMCKey::setKey(bool key)
{
    if(key == _key) return;
    _key = key;
    emit keyChanged(_key);

    if(_key){
        emit press(this);
        _accumulatedTime = 1;
        timer->start();
    }else{
        emit upspring(this);
        timer->stop();
        if(_accumulatedTime  < 5) emit click(this);
    }
}

void MMCKey::onTimeOut()
{
    _accumulatedTime++;
    emit longPress(this);
}

//------------------------------------------------[分割线]----------------------------------------------------
RadioMemberBase::RadioMemberBase(QObject *parent)
    : QObject(parent)
    , _checkStatus(No_Check)
    , _chargeState(0)
    , _time(0)
    , _stateOfHealth(0)
    , _temperature(0)
    , _rockerState(1)
    , _rcMode(0)
    , _ver("")
    , _calirationState(0)
    , _channel1(1500)
    , _channel2(1500)
    , _channel3(1500)
    , _channel4(1500)
    , _channelBMax1(0)
    , _channelBMax2(0)
    , _channelBMax3(0)
    , _channelBMax4(0)
    , _channelBMin1(0)
    , _channelBMin2(0)
    , _channelBMin3(0)
    , _channelBMin4(0)
    , _channelBMed1(0)
    , _channelBMed2(0)
    , _channelBMed3(0)
    , _channelBMed4(0)
{
    /* 同步电台需要主动获取数据的计时器 */
    _syncDataTimer = new QTimer(this);
    _syncDataTimer->setInterval(300);
    connect(_syncDataTimer, SIGNAL(timeout()), this, SLOT(onSyncData()));

}

void RadioMemberBase::setRadioID(QString id)
{
    qDebug() << QString("-------------------radioID") << id;
    if(_radioID == id)  return;
    _radioID = id;
    emit radioIDChanged();
}

void RadioMemberBase::setRadioLock(bool ck)
{
    int lock;
    if(ck)
        lock = 1;
    else
        lock = 0;

    if(_radioLock == lock) return;
    _radioLock = lock;
    emit radioLockChanged();

    if(lock){ //串口上线时
        _syncDataTimer->start();
    }
}

void RadioMemberBase::setVer(uchar *buff)
{
    uint32_t tmp = 0;
    uint32_t version = 0;
    memcpy(&tmp, buff, sizeof(uint32_t));
    memcpy(&version, &tmp, sizeof(uint32_t));
    int versionHardware     = (int) ((version >> 30) & 0x3);
    int versionFunction     = (int) ((version >> 24) & 0x3f);
    int versionOptimization = (int) ((version >> 16) & 0xff);
    int versionYear         = (int) ((version >> 9) & 0x7f);
    int versionMonth        = (int) ((version >> 5) & 0xf);
    int versionDay          = (int) ((version >> 0) & 0x1f);
    int versionTime         = versionYear*10000 + versionMonth * 100 + versionDay;
    QString vers            = QString("%1.%2.%3.%4").arg(versionHardware).arg(versionFunction).arg(versionOptimization).arg(versionTime);
    set_ver(vers);
}


/* **********************************************************************
 * 下发指令
 * **********************************************************************/
void RadioMemberBase::onSyncData()
{
    int i = 0;
    if(_radioID.isEmpty()){
        queryRadioId(); i++;
    }

    if(i == 0) _syncDataTimer->stop(); //所有数据都已经存在，关闭定时器
}

void RadioMemberBase::queryRadioId()
{
    return;
    char type = 0x8f;
    char buff[1] = {0x01};
    emit _writeData(type, QByteArray(buff, 1));
}

void RadioMemberBase::setCalirationState(bool isLeft)
{
    char type = 0x2f;
    char buff[1] = {L_MODE};
    if(!isLeft)
        buff[0] = R_MODE;
    emit _writeData(type, QByteArray(buff, 1));
}

void RadioMemberBase::sendCheckStatus()
{
    CheckStatus checkState = (CheckStatus)checkStatus();
    if(checkState < 6 && checkState % 2){ //单数 重复本次校准
    }else if(checkState == No_Check || checkState == ACK_CALIBRATION_COMPLETED || checkState == ERRORS){ //开始新的一轮校准
        set_checkStatus(IN_CALIBRATION);
    }else if(checkState < 6 && !(checkState % 2)){ //+1 下一步
        set_checkStatus((CheckStatus)(checkState+1));
    }else{//不在取值范围
        return;
    }
    char type = 0x4f;
    char buff[1] = {checkStatus()};
    qDebug() << "-------------------sendCheckStatus" << type <<QByteArray(buff, 1).toHex();
    emit _writeData(type, QByteArray(buff, 1));
}

void RadioMemberBase::rockerControl(int state)
{
    if(state != 0 && state != 1) return;
    char type = 0x6f;
    char buff[1] = {state};

    emit _writeData(type, QByteArray(buff, 1));
}
