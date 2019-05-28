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

#include <QProcess>
RadioMember::RadioMember(QObject *parent)
    : RadioMemberBase(parent)
{
    for(int i=0; i < 8; i++){
        _key[i] = new MMCKey(i+1, this);
        connect(_key[i], SIGNAL(press(MMCKey*)), this, SLOT(onPress(MMCKey*)));
        connect(_key[i], SIGNAL(upspring(MMCKey*)), this, SLOT(onUpspring(MMCKey*)));
        connect(_key[i], SIGNAL(longPress(MMCKey*)), this, SLOT(onLongPress(MMCKey*)));
        connect(_key[i], SIGNAL(click(MMCKey*)), this, SLOT(onClick(MMCKey*)));
    } 
    keyBindFunction();

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
    set_energy(energy);
}

void RadioMember::setVoltage(uchar value) //130~168
{
    int voltage = (uchar)value;
    setVoltage((float)voltage/10);
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



void RadioMember::_say(const QString &text)
{
    qgcApp()->toolbox()->audioOutput()->say(text.toLower());
}

void RadioMember::setVoltage(float value)
{
    float voltag = voltage();
    if(voltag == value) return;
    set_voltage(value);
    voltag = voltage();

    static float minVoltage = 13.6;
    if(voltag < minVoltage){
        minVoltage = voltag;
        if(_lang == "Chinese"){
            _say(QString("低电压，%1伏").arg(voltag));
        }else{
            _say(QString("Low Voltage,%1V").arg(voltag));
        }

    }else if(voltag > 14 &&  minVoltage < 13.4){ //表示充了电
        minVoltage = 13.6;
    }
}

/* **********************************************************************
 * 下发指令
 * **********************************************************************/

void RadioMember::radioControl(int state)
{
    if(state != 0 && state != 1 /*&& state != 2*/) return;

    char type = 0x7f;
    char buff[1] = {state};
    emit _writeData(type, QByteArray(buff, 1));
}



void RadioMember::analysisPack(int type, QByteArray msg)
{
    uchar* buff = (uchar*)msg.data();
    ushort tep = 0;
    float tmp;
    if(type == 0x5f)
//    qDebug() << "---------------------- RadioMember::analysisPack" << type << msg.toHex();
    switch (type) {
    case 0x1f: { //遥控器各通道 -- 16字节
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel1(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel2(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel3(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel4(tep);
        buff += 2;
        for(int i = 0; i < 8; i++){
            this->setKey(i, *buff++);
        }
        break;
        }
    case 0x3f:{  //心跳
        this->set_chargeState(*buff++);
        this->setVoltage(*buff++);
        this->setQuantity(*buff++);
        this->set_time(((float)*buff++)/10);
        this->set_stateOfHealth(*buff++);
        memcpy(&tmp, buff, sizeof(float));
        this->set_temperature(tmp);
        buff += sizeof(float);
        this->set_rockerState((*buff) & 0x01);
        this->setRadioSourceState(((*buff)>>1 & 0x01));
        this->setLidState(((*buff)>>2 & 0x01));
        buff++;
        this->set_rcMode(*buff++);
        this->set_calirationState(*buff++);
        this->setVer(buff);
        buff += 4;
        break;
    }
    case 0x4f:{  //遥控器校准时各通道值
        this->set_checkStatus(*buff++);
        break;
    }
    case 0x5f:{  //遥控器校准时各通道值
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMax1(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMax2(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMax3(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMax4(tep);
        buff += 2;

        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMid1(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMid2(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMid3(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMid4(tep);
        buff += 2;

        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMin1(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMin2(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMin3(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMin4(tep);
        buff += 2;
        break;
    }
    case 0x8f:{  //单片机唯一ID
        this->setRadioID(QByteArray((char*)buff, 12));
        break;
    }
    default:
        break;
    }
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


