#include "radiomember.h"

#include "QGCApplication.h"
//#include "MMCMount/pg2if1cameramount.h"
//#include "MMCMount/filrcameramount.h"
//#include "MMCMount/pgiy1cameramount.h"

#include <QNetworkInterface>
#include "stdio.h"

#include "QGCConfig.h"

//控制屏幕亮度库
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")

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
#include <QProcess>
RadioMember::RadioMember(QObject *parent)
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
    for(int i=0; i < 8; i++){
        _key[i] = new MMCKey(i+1, this);
        connect(_key[i], SIGNAL(press(MMCKey*)), this, SLOT(onPress(MMCKey*)));
        connect(_key[i], SIGNAL(upspring(MMCKey*)), this, SLOT(onUpspring(MMCKey*)));
        connect(_key[i], SIGNAL(longPress(MMCKey*)), this, SLOT(onLongPress(MMCKey*)));
        connect(_key[i], SIGNAL(click(MMCKey*)), this, SLOT(onClick(MMCKey*)));
    } 
    keyBindFunction();

    /* 同步电台需要主动获取数据的计时器 */
    _syncDataTimer = new QTimer(this);
    _syncDataTimer->setInterval(300);
    connect(_syncDataTimer, SIGNAL(timeout()), this, SLOT(onSyncData()));

    QSettings settings;
    settings.beginGroup("USER_DEFINE_CONFIG");
    _lang = settings.value("Language").toString();
    settings.endGroup();

    QMap<uint, QString> enumToString;
    if(_lang == "Chinese")
    {
        enumToString.insert(KEY_UNDEFINED,                  QStringLiteral("未定义"));
        enumToString.insert(KEY_VOLUME_UP,                  QStringLiteral("音量加"));
        enumToString.insert(KEY_VOLUME_DOWN,                QStringLiteral("音量减"));
        enumToString.insert(KEY_BRIGHTNESS_UP,              QStringLiteral("亮度加"));
        enumToString.insert(KEY_BRIGHTNESS_DOWN,            QStringLiteral("亮度减"));
        enumToString.insert(KEY_VEHICLE_AUTO_MODE,          QStringLiteral("航线模式"));
        enumToString.insert(KEY_VEHICLE_LOITER_MODE,        QStringLiteral("悬停模式"));
    }else{
        enumToString.insert(KEY_UNDEFINED,                  QStringLiteral("Undefined"));
        enumToString.insert(KEY_VOLUME_UP,                  QStringLiteral("Volume Up"));
        enumToString.insert(KEY_VOLUME_DOWN,                QStringLiteral("Volume Down"));
        enumToString.insert(KEY_BRIGHTNESS_UP,              QStringLiteral("Gamma Up"));
        enumToString.insert(KEY_BRIGHTNESS_DOWN,            QStringLiteral("Gamma Down"));
        enumToString.insert(KEY_VEHICLE_AUTO_MODE,          QStringLiteral("Auto"));
        enumToString.insert(KEY_VEHICLE_LOITER_MODE,        QStringLiteral("Loiter"));
    }
    setEnumToStringMapping(enumToString);

    _mmcStationID = getHostMacAddress(); //并不唯一  后续改为  -  硬盘序号+网卡序号+主板序号 加密算法
}

void RadioMember::setEnumToStringMapping(const QMap<uint, QString> &enumToString)
{
    _enumToString = enumToString;
}

QStringList RadioMember::keyModes()
{
    QStringList keyModes;

    QMap<uint, QString>::const_iterator i = _enumToString.constBegin();
     while (i != _enumToString.constEnd()) {
//         cout << i.key() << ": " << i.value() << endl;
         keyModes << i.value();
         ++i;
     }
     return keyModes;
}


/* **********************************************************************
 * 接收数据 -- 数据转换
 * **********************************************************************/

void RadioMember::setKey(int i, uchar key)
{
    if(i >= 0 && i < 8)
        _key[i]->setKey((key & 0x02));
}

void RadioMember::setQuantity(uchar value)
{
    int energy = (uchar)value;
    if(energy > 100) energy = 100;
    setEnergy(energy);
}

void RadioMember::setVoltage(uchar value) //130~168
{
    int voltage = (uchar)value;
//    if(voltage > 168) voltage = 168;
//    if(voltage < 130) voltage = 130;
    setVoltage((float)voltage/10);
}

void RadioMember::setRadioLock(bool ck)
{
    int lock;
    if(ck)
        lock = 1;
    else
        lock = 0;

    if(_radioLock == lock) return;
    _radioLock = lock;
    emit radioLockChanged(_radioLock);

    if(lock){ //串口上线时
        _syncDataTimer->start();
    }
}

void RadioMember::setRadioSourceState(int state)
{
    int statu = 0;
    if(state != 0) statu = 1;
    if(_radioSourceState == statu) return;
    qDebug() << QString("-------------------radioSourceState") << state;
    _radioSourceState = statu;
    emit radioSourceStateChanged(_radioSourceState);
}

void RadioMember::setLidState(int state)
{
    int statu = 0;
    if(state != 0) statu = 1;
//    if(_radioSourceState == statu) return;
//    qDebug() << QString("-------------------setLidState") << state;
}

void RadioMember::setRadioID(QString id)
{
    qDebug() << QString("-------------------radioID") << id;
    if(_radioID == id)  return;
    _radioID = id;
    emit radioIDChanged();
}

void RadioMember::_say(const QString &text)
{
    qgcApp()->toolbox()->audioOutput()->say(text.toLower());
}

void RadioMember::setEnergy(int value)
{
    if(_energy == value) return;
    _energy = value;
    emit energyChanged(_energy);
}

void RadioMember::setVoltage(float value)
{
    if(_voltage == value) return;
    _voltage = value;
    emit voltageChanged();

    static float minVoltage = 13.6;
    if(_voltage < minVoltage){
        minVoltage = _voltage;
        if(_lang == "Chinese"){
            _say(QString("低电压，%1伏").arg(_voltage));
        }else{
            _say(QString("Low Voltage,%1V").arg(_voltage));
        }

    }else if(_voltage > 14 &&  minVoltage < 13.4){ //表示充了电
        minVoltage = 13.6;
    }
}

/* **********************************************************************
 * 下发指令
 * **********************************************************************/
void RadioMember::onSyncData()
{
    int i = 0;
    if(_radioID.isEmpty()){
        queryRadioId(); i++;
    }
//    if(_queryNumber < 15){ //一次start 查询20次
//        radioControl(2); i++; _queryNumber++;
//    }

    if(i == 0) _syncDataTimer->stop(); //所有数据都已经存在，关闭定时器
}

void RadioMember::rockerControl(int state)
{
    if(state != 0 && state != 1) return;

//    if(state == 0 || state == 1){ //由于每次开启 关闭 都要主动去查询状态  故加此if
//        _queryNumber = 0;
//        _syncDataTimer->start();
//    }

    char type = 0x6f;
    char buff[1] = {state};
//    QByteArray array.

    emit _writeData(type, QByteArray(buff, 1));
}

void RadioMember::radioControl(int state)
{
    if(state != 0 && state != 1 /*&& state != 2*/) return;

//    if(state == 0 || state == 1){ //由于每次开启 关闭 都要主动去查询状态  故加此if
//        _queryNumber = 0;
//        _syncDataTimer->start();
//    }

    char type = 0x7f;
    char buff[1] = {state};
    emit _writeData(type, QByteArray(buff, 1));
}

void RadioMember::sendCheckStatus()
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

void RadioMember::setCalirationState(bool isLeft)
{
    char type = 0x2f;
    char buff[1] = {L_MODE};
    if(!isLeft)
        buff[0] = R_MODE;
    emit _writeData(type, QByteArray(buff, 1));
}

void RadioMember::queryRadioId()
{
    return;
    char type = 0x8f;
    char buff[1] = {0x01};
    emit _writeData(type, QByteArray(buff, 1));
}

/* **********************************************************************
 * 按键方法控制
 * **********************************************************************/

int RadioMember::keyBinding(int key, QString type)
{
    /* 1-缩小  2-放大  3-自定义1  4-自定义2  5-FN  6-云台模式   7-录像   8-拍照  */
    if(key < 1 || key > 8) return -1;

    QMap<uint, QString>::const_iterator i = _enumToString.constBegin();
    int value = -1;
     while (i != _enumToString.constEnd()) {
         if(i.value() == type){
                value = i.key();
                break;
         }
         ++i;
     }

     if(value != -1){
         _keyId[key] = (KEY_TYPE)value; //此处还要判断一次value是否在枚举范围内

         QSettings settings;
         settings.beginGroup("MMC_KEYS_TYPE");
         settings.setValue(QString("MMC_KEY%1").arg(key),    (KEY_TYPE)value);
         settings.endGroup();
     }else
         return -2;
     return 1;
}

QString RadioMember::getKeyValue(int key)
{
    if(key < 1 || key > 8) return "";
    if(_enumToString.contains(_keyId[key])){ //包含此键
            return _enumToString[_keyId[key]];
    }
    return "";
}

void RadioMember::setVer(uchar *buff)
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

void RadioMember::keyBindFunction()
{
    /* 按键与功能绑定 -- 后面可用配置对应*/
    _keyId[1] =KEY_ZOOM_DOWN;      //缩小
    _keyId[2] =KEY_ZOOM_UP;        //放大
    _keyId[5] =KEY_FN;             //FN
    _keyId[6] =KEY_MODE;           //云台模式
    _keyId[7] =KEY_REC;            //录像
    _keyId[8] =KEY_PHOTO;          //拍照

    QSettings settings; //读取ini
    settings.beginGroup("MMC_KEYS_TYPE");
    _keyId[3] = settings.value(QString("MMC_KEY%1").arg(3), KEY_UNDEFINED).toInt();
    _keyId[4] = settings.value(QString("MMC_KEY%1").arg(4), KEY_UNDEFINED).toInt();
    settings.endGroup();
}

void RadioMember::onPress(MMCKey *key)
{
    switch (_keyId[key->id()]) {
    case KEY_ZOOM_DOWN: //缩小
        zoomDown();
        break;
    case KEY_ZOOM_UP:        //放大
        zoomUp();
        break;
//    case KEY_CUSTOM_1:       //自定义1
//        break;
//    case KEY_CUSTOM_2:       //自定义2
//        break;
//    case KEY_FN:             //FN
//        break;
    case KEY_MODE:           //云台模式
        break;
    case KEY_PHOTO:          //拍照
        break;
    case KEY_REC:            //录像
        break;
    default:
        break;
    }
}

void RadioMember::onUpspring(MMCKey *key)
{
    switch (_keyId[key->id()]) {
    case KEY_ZOOM_DOWN: //缩小
        zoomStop();
        break;
    case KEY_ZOOM_UP:        //放大
        zoomStop();
        break;
//    case KEY_CUSTOM_1:       //自定义1
//        break;
//    case KEY_CUSTOM_2:       //自定义2
//        break;
//    case KEY_FN:             //FN
//        break;
    case KEY_MODE:           //云台模式  -- 长按压
        if(key->accumulatedTime()  > 2) locking();
        break;
    case KEY_PHOTO:          //拍照
        break;
    case KEY_REC:            //录像
        break;
    default:
        break;
    }
}

void RadioMember::onLongPress(MMCKey *key)
{
    switch (_keyId[key->id()]) {
    case KEY_ZOOM_DOWN: //缩小
        zoomDown();
        break;
    case KEY_ZOOM_UP:        //放大
        zoomUp();
        break;
//    case KEY_CUSTOM_1:       //自定义1
//        break;
//    case KEY_CUSTOM_2:       //自定义2
//        break;
//    case KEY_FN:             //FN
//        break;
    case KEY_MODE:           //云台模式
        break;
    case KEY_PHOTO:          //拍照
        break;
    case KEY_REC:            //录像
        break;
    default:
        break;
    }
}

void RadioMember::onClick(MMCKey *key)
{
    switch (_keyId[key->id()]) {
    case KEY_ZOOM_DOWN: //缩小
        break;
    case KEY_ZOOM_UP:        //放大
        break;
//    case KEY_CUSTOM_1:       //自定义1
//        break;
//    case KEY_CUSTOM_2:       //自定义2
//        break;
    case KEY_FN:             //FN -- 复合功能
        fnClick();
        break;
    case KEY_MODE:           //云台模式
        Mode();
        break;
    case KEY_PHOTO:          //拍照
        photo();
        break;
    case KEY_REC:            //录像
        REC();
        break;

    /* 其它扩展功能 -- 服务自定义功能键 */
    case KEY_UNDEFINED:                     //未定义
        break;
    case KEY_VOLUME_UP:                     //音量加
        upVolume();
        break;
    case KEY_VOLUME_DOWN:                   //音量减
        downVolume();
        break;
    case KEY_BRIGHTNESS_UP:                 //亮度加
        upGamma();
        break;
    case KEY_BRIGHTNESS_DOWN:               //亮度减
        downGamma();
        break;
    case KEY_VEHICLE_AUTO_MODE:             //飞机航线模式
        autoMode();
        break;
    case KEY_VEHICLE_LOITER_MODE:           //飞机悬停模式
        loiterMode();
        break;
    default:
        break;
    }
}

/* **********************************************************************
 * 按键可用方法
 * **********************************************************************/
void RadioMember::photo()
{
//    Vehicle* activeVehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();
//    if(activeVehicle){
//        activeVehicle->doCameraTrigger();
//    }
}

void RadioMember::REC()
{
//    Vehicle* activeVehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();
//    if(activeVehicle && !activeVehicle->mountLost() && activeVehicle->currentMount() &&activeVehicle->currentMount()->mountType() == MountInfo::MOUNT_CAM_INFO){
//        CameraMount* camMount = dynamic_cast<CameraMount*>(activeVehicle->currentMount());
//        camMount->videoTape();
//    }
}

void RadioMember::Mode()
{
//    Vehicle* activeVehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();
//    if(activeVehicle && !activeVehicle->mountLost() && activeVehicle->currentMount() &&activeVehicle->currentMount()->mountType() == MountInfo::MOUNT_CAM_INFO){
//        CameraMount* camMount = dynamic_cast<CameraMount*>(activeVehicle->currentMount());
//        int mode = camMount->mode();
//        if(mode == 0) mode = 2;
//        else mode = 3 - mode;
//        camMount->controlMode(mode);
//    }
}

void RadioMember::locking()
{
//    Vehicle* activeVehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();
//    if(activeVehicle && !activeVehicle->mountLost() && activeVehicle->currentMount() &&activeVehicle->currentMount()->mountType() == MountInfo::MOUNT_CAM_INFO){
//        CameraMount* camMount = dynamic_cast<CameraMount*>(activeVehicle->currentMount());
//        int mode = camMount->mode();
//        if(mode != 0){
//           mode = 0;
//          camMount->controlMode(mode);
//        }
//    }
}

void RadioMember::zoomUp()
{
    if(_zoom == 0) return;
    _zoom = 2;
    controlZoom(_zoom);
}

void RadioMember::zoomDown()
{
    if(_zoom == 2) return;
    _zoom = 0;
    controlZoom(_zoom);
}

void RadioMember::zoomStop()
{
    _zoom = 1;
    controlZoom(_zoom);
}

void RadioMember::controlZoom(int zoom)
{
//    Vehicle* activeVehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();
//    if(activeVehicle && !activeVehicle->mountLost() && activeVehicle->currentMount() &&activeVehicle->currentMount()->mountType() == MountInfo::MOUNT_CAM_INFO){
//        CameraMount* camMount = dynamic_cast<CameraMount*>(activeVehicle->currentMount());
//        camMount->controlZoom(zoom);
//    }
}

void RadioMember::fnClick()
{
  /*  Vehicle* activeVehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();
    if(activeVehicle && !activeVehicle->mountLost() && activeVehicle->currentMount()){
        switch (activeVehicle->currentMount()->mountType()){
        case MountInfo::MOUNT_SPEAKE: //喊话器   --  [禁音与恢复] -- 该挂载不需要此功能
            if(1){

            }
            break;
        case MountInfo::MOUNT_4GSPEAKE: //4G喊话器   --  [禁音与恢复]
            if(1){
                Speake4GMount* mount = dynamic_cast<Speake4GMount*>(activeVehicle->currentMount());
                static int volume = 0;
                if(mount->volume() != 0) {
                    volume = mount->volume();
                    mount->volumeControl(0);
                }else{
                    mount->volumeControl(volume);
                    volume = 0;
                }
            }
            break;
        case MountInfo::MOUNT_LIGHT: //探照灯  --  [开关灯]
            if(1){
                LightMount* mount = dynamic_cast<LightMount*>(activeVehicle->currentMount());
                mount->lightControl(1 - mount->state() != 0);
            }
            break;

        case MountInfo::MOUNT_DROP: //抛投  -- [投掷]
            if(1){
                DropMount* mount = dynamic_cast<DropMount*>(activeVehicle->currentMount());
                mount->dropCmmd();
            }
            break;
        case MountInfo::MOUNT_CAM_INFO:
            switch (dynamic_cast<CameraMount*>(activeVehicle->currentMount())->cam_type()){
            case CameraMount::CAM_SONGXIA20: //20倍松下云台  -- [相机模式]
                if(1){
                    CameraMount* mount = dynamic_cast<CameraMount*>(activeVehicle->currentMount());
                    mount->switchCameraMode();
                }
                break;

            case CameraMount::CAM_PG2IF1_LS1Z20: //高清双光 20b
                if(1){
                    PG2IF1CameraMount* mount = dynamic_cast<PG2IF1CameraMount*>(activeVehicle->currentMount());
                    mount->controlFrame(1 - mount->frame());
                }
                break;

            case CameraMount::CAM_Filr: //Filr红外
            case CameraMount::CAM_PGIY1: //海视英科红外
                if(1){
                    CameraMount* mount = dynamic_cast<CameraMount*>(activeVehicle->currentMount());
                    mount->switchColor();
                }
                break;
            }
            break;
        }
    } */
}

/* 飞机模式切换 */
void RadioMember::autoMode()
{
    setVehicleMode(KEY_VEHICLE_AUTO_MODE);
}

void RadioMember::loiterMode()
{
    setVehicleMode(KEY_VEHICLE_LOITER_MODE);
}

void RadioMember::setVehicleMode(RadioMember::KEY_TYPE keyType)
{
    Vehicle* activeVehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();
    if(activeVehicle){
        QString flightMode =  activeVehicle->flightMode();
        QString mode = _enumToString[keyType];
            if(flightMode != mode)
                activeVehicle->setFlightMode(mode);
    }
    //    QStringList flightModes(void);
}

int RadioMember::setGamma(int direction)
{

#if defined(Q_OS_WIN32)
    void * lpGamma = NULL;
    int iArrayValue = 0;
    int bright = 0;
    WORD gMap[3][256] = {0};
    lpGamma = &gMap;
    HDC hdc = ::GetDC(NULL);
    if (NULL == hdc)
        return -1;

    if (FALSE == GetDeviceGammaRamp(hdc, lpGamma))
                return -2;

    for (int i = 0; i < 256; i++)
    {
        for(int j=0; j < 3; j++){

           if(i > 0){
               bright = gMap[j][i] / i - 128;
           }else{
               bright = gMap[j][i]  - 128;
           }

           bright += direction;
           if (bright > 255)
                bright = 255;
           if (bright < 0)
                bright = 0;
           iArrayValue = i * (bright + 128);
           if (iArrayValue > 65535)
               iArrayValue = 65535;
           gMap[j][i] = (WORD)iArrayValue;
        }
    }

    if (FALSE == SetDeviceGammaRamp(hdc, lpGamma))
        return -3;

#else
    qDebug()<<"Q_OS_other";
#endif

    return 0;
}

void RadioMember::upGamma()
{
    setGamma(20);
}

void RadioMember::downGamma()
{
    setGamma(-20);
}

void RadioMember::setVolume(bool isUp, uchar direction)
{
//    DWORD volume;
//    waveOutGetVolume(0, &volume);
//    ushort value = LOWORD(volume); //高位为左声道，低位为右声道
//    uchar  leftVolume = (value >> 8) & 0xff;
//    uchar  rightVolume = value & 0xff;
//    if(isUp){
//        if(leftVolume  > 255 - direction)
//            leftVolume = 255;
//        else
//            leftVolume  += direction;
//        if((ushort)rightVolume >255 - direction)
//            rightVolume = 255;
//        else
//            rightVolume += direction;
//    }else{
//        if(leftVolume < direction )
//            leftVolume = 0;
//        else
//            leftVolume  -= direction;

//        if(rightVolume < direction )
//            rightVolume = 0;
//        else
//            rightVolume -= direction;
//    }

//    value = (((ushort)leftVolume << 8) & 0xff00) + (rightVolume & 0xff);
//    waveOutSetVolume(0, MAKELONG(value, value));
}

void RadioMember::upVolume()
{
    setVolume(true, 25);
}

void RadioMember::downVolume()
{
    setVolume(false, 25);
}

/* **********************************************************************
 * cpu唯一码
 * **********************************************************************/

void RadioMember::getCpuId(unsigned int CPUInfo[], unsigned int InfoType)
{

}

void RadioMember::getCpuIdex(unsigned int CPUInfo[], unsigned int InfoType, unsigned int ECXValue)
{
//#if defined(_MSC_VER) // MSVC
//#if defined(_WIN64) // 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.
//    __cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
//#else
//    if (NULL==CPUInfo)  return;
//    _asm{
//        // load. 读取参数到寄存器.
//        mov edi, CPUInfo;
//        mov eax, InfoType;
//        mov ecx, ECXValue;
//        // CPUID
//        cpuid;
//        // save. 将寄存器保存到CPUInfo
//        mov [edi], eax;
//        mov [edi+4], ebx;
//        mov [edi+8], ecx;
//        mov [edi+12], edx;
//    }
//#endif
//#endif
}

QString RadioMember::getCpuId()
{
    QString cpu_id = "";
    unsigned int dwBuf[4]={0};
    unsigned long long ret = 0;
    getCpuId(dwBuf, 1);
    ret = dwBuf[3];
    ret = ret << 32;

    QString str0 = QString::number(dwBuf[3], 16).toUpper();
    QString str0_1 = str0.rightJustified(8,'0');//这一句的意思是前面补0，但是我遇到的情况是这里都填满了
    QString str1 = QString::number(dwBuf[0], 16).toUpper();
    QString str1_1 = str1.rightJustified(8,'0');//这里必须在前面补0，否则不会填满数据
    //cpu_id = cpu_id + QString::number(dwBuf[0], 16).toUpper();
    cpu_id = str0_1 + str1_1;
    return cpu_id;
}

QString RadioMember::getCpuId2()
{
    QString cpu_id = "";
    QProcess p(0);
    p.start("wmic CPU get ProcessorID");    p.waitForStarted();
    p.waitForFinished();
    cpu_id = QString::fromLocal8Bit(p.readAllStandardOutput());
    cpu_id = cpu_id.remove("ProcessorId").trimmed();
    return cpu_id;
}

QString RadioMember::getHostMacAddress()
{
    QString strMacAddr = "";
#ifdef Q_OS_WIN32
    QSettings settings(QGC_ORG_NAME, QGC_APPLICATION_NAME);
    strMacAddr = settings.value("strMacAddr", "").toString();//读
    if(!strMacAddr.isEmpty()) return strMacAddr;
#endif

    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
       int nCnt = nets.count();
       for(int i = 0; i < nCnt; i ++)
       {
           strMacAddr = nets[i].hardwareAddress();
           break;
       }
#ifdef Q_OS_WIN32
    if(!strMacAddr.isEmpty())
        settings.setValue("strMacAddr", strMacAddr);//写
#endif
       return strMacAddr;
}


