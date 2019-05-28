#include "cameramount.h"
#include "QGCApplication.h"
#include "Vehicle.h"

CameraMount::CameraMount(Vehicle* vehicle)
    : MountInfo(vehicle)
    , _zoom(0)
    , _pitchAngle(0.0)
    , _rollAngle(0.0)
    , _headAngle(0.0)
    , _mode(-1)
    , _tackPictIndex(0)
    , _backCode("")
    , _zoomState(1)
    , _writeROMState(false)
    , _writeFlashState(false)
{
    _dataPIDList << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0);

    _dataPIDListM2 << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0);

    _dataPIDListM3 << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0)
                << QVariant::fromValue(0);

//    MMCSettings::dSetValue("app/IP Port","80","LinkConfigurations",true);
//    _port = MMCSettings::dValue("app/IP Port","","LinkConfigurations",true).value<QString>();

    _debugOnline = true;// MMCSettings::dValue("mount/debugOnline", "false", "Mount", true).value<QString>() == "true"; //MMCSettings::getvalue("mount/debugOnline","").toString()
    emit debugOnlineChanged();

    _timer = new QTimer(this);
    _timer->setInterval(650); // 800的超时
    _timer->setSingleShot(true);
    connect(_timer, &QTimer::timeout, this, &CameraMount::onSwitchColorSend);
}

void CameraMount::setZoom(int zoom)
{
    if(_zoom == zoom)
        return;
    _zoom = zoom;
    emit zoomChanged();
}

void CameraMount::setZoomPosition(int value)
{
    if(_zoomPosition == value)
        return;
    _zoomPosition = value;
    emit zoomPositionChanged();
}

void CameraMount::setCam_type(int type)
{
    if(_cam_type == type)
        return;
    _cam_type = type;
    emit cam_typeChanged();
}

void CameraMount::setDebugOnline(bool debugOnline)
{
    if(_debugOnline == debugOnline)
        return;
    _debugOnline = debugOnline;

//    if(_debugOnline)
//        MMCSettings::dSetValue("mount/debugOnline", "true", "Mount", true);
//    else
//        MMCSettings::dSetValue("mount/debugOnline", "false", "Mount", true);
    emit debugOnlineChanged();
}

void CameraMount::setPitchAngle(float angle)
{
    if(_pitchAngle == angle)
        return;
    _pitchAngle = angle;
    emit pitchAngleChanged();
}
void CameraMount::setRollAngle(float angle)
{
    if(_rollAngle == angle)
        return;
    _rollAngle = angle;
    emit rollAngleChanged();
}
void CameraMount::setHeadAngle(float angle)
{
    if(_headAngle == angle)
        return;
    _headAngle = angle;
    emit headAngleChanged();
}
void CameraMount::setVideoStatus(int status)
{
    if(_videoStatus == status)
        return;
    _videoStatus = status;
    emit videoStatusChanged(_videoStatus);
}
void CameraMount::setMode(int mode)
{
    if(_mode == mode)
        return;
    _mode = mode;
    emit modeChanged();
}
void CameraMount::setErrorCode(int err)
{
    if(_errorCode == err)
        return;
    _errorCode = err;
    emit errorCodeChanged();
}

void CameraMount::setHasError(bool isError)
{
    if(_hasError == isError)
        return;
    _hasError = isError;
    emit hasErrorChanged();
}

void CameraMount::setZoomState(int zoomState)
{
    _zoomState = zoomState;
    qDebug() << "----------setZoomState" << zoomState;
    emit zoomStateChanged();
}

void CameraMount::setBackCode(QString code)
{
    if(_backCode == code)
        return;
    _backCode = code;
    emit backCodeChanged();
}
void CameraMount::setTackPictIndex(int index)
{
    if(_tackPictIndex == index)
        return;
    _tackPictIndex = index;
    emit tackPictIndexChanged();
}

void CameraMount::setZoomUseState(int zoomUseState)
{
    _zoomUseState = zoomUseState;
    emit zoomUseStateChanged();
}

void CameraMount::setVideoUseClick(int videoUseClick)
{
    _videoUseClick = videoUseClick;
    emit videoUseClickChanged();
}

void CameraMount::oninitVideoUseClick()
{
    setVideoUseClick(1);
}

void CameraMount::controlZoom(int zoom)
{
    if(2 - _zoomUseState == zoom) return;  //放大时禁止缩小  缩小时禁止放大  停止时禁止停止
    setZoomUseState(zoom);

    uint8_t buff[5] = {0xA5,CAM_DN_CHANGEZOOM,3,2,0};
    buff[3] = zoom;
    qDebug() << "--------------------------CameraMount::controlZoom" << zoom;
    sendData(buff, 5);
}

void CameraMount::controlPitch(int offset, bool plus)
{
    uint8_t buff[5]={0xA5,CAM_DN_PITCH,3,0,0};
    if(!plus)
        buff[3] = -offset;
    else
        buff[3] = offset;
    qDebug() << "--------------------------CameraMount::controlPitch" << offset << plus;
    sendData(buff, 5);
}

void CameraMount::controlPitchAngle(int angle)//角度控制
{
    if(angle == (int)_pitchAngle)
        return;

    uint8_t buff[5]={0xA5,CAM_DN_PITCHANGLE,3,0,0};
    buff[3] = angle;
    qDebug() << "--------------------------CameraMount::controlPitchAngle" << angle;
    sendData(buff, 5);
}

void CameraMount::controlHeadAngle(int angle)//角度控制
{
    if(angle == (int)_headAngle)
        return;

    uint8_t buff[6]={0xA5,CAM_DN_HEADANGLE,4,0,0,0};
    memcpy(&buff[3], (int16_t *)&angle, 2);
    qDebug() << "--------------------------CameraMount::controlHeadAngle" << angle;
    sendData(buff, 6);
}

void CameraMount::controlHead(int offset, bool plus)
{
    uint8_t buff[5]={0xA5,CAM_DN_HEAD,3,0,0};
    if(!plus)
        buff[3] = -offset;
    else
        buff[3] = offset;
    qDebug() << "--------------------------CameraMount::controlHead" << offset << plus;
    sendData(buff, 5);
}

void CameraMount::videoTape(void)
{
    setVideoUseClick(2);
    QTimer::singleShot(200, this, SLOT(oninitVideoUseClick()));

    //------ yin wu shang xing shu ju , suo yi gai wei jin tai bian liang
    static bool Status = false;
    uint8_t buff[5] = {0xA5,5,3,2,0};
    if(Status){
        Status = false;
    }else{
        Status = true;
        buff[3] = 0;
    }
    qDebug() << "--------------------------CameraMount::videoTape" << Status;
    sendData(buff, 5);


//    uint8_t buff[5] = {0xA5,5,3,2,0};
//    if(_videoStatus == 1)
//        buff[3] = 0;
//    qDebug() << "--------------------------CameraMount::videoTape" << _videoStatus;
//    sendData(buff, 5);
}

void CameraMount::controlMode(int mode)
{
    if(mode == _mode)
        return;
    uint8_t buff[5]={0xA5,CAM_DN_MODE,3,0,0};
    buff[3] = mode; //0:锁航向  1:自由控 2：一键回中
    qDebug() << "--------------------------CameraMount::controlMode" << mode;
    sendData(buff, 5);
}

void CameraMount::controlPos(int x, int y, int type)
{
    uint8_t buff[13]={0xA5,CAM_DN_POSCONTROL,11,0,0,0,0,0,0,0,0,0,0};
    buff[3] = type;
    memcpy(&buff[4], (int16_t *)&x, 2);
    memcpy(&buff[6], (int16_t *)&y, 2);
    qDebug() << "--------------------------CameraMount::controlPos" << x << y << type;
    sendData(buff, 13);
}

void CameraMount::controlPos(int x, int y, int width, int height,int type)
{
    uint8_t buff[13]={0xA5,CAM_DN_POSCONTROL,11,0,0,0,0,0,0,0,0,0,0};
    buff[3] = type;
    memcpy(&buff[4], (int16_t *)&x, 2);
    memcpy(&buff[6], (int16_t *)&y, 2);
    memcpy(&buff[8], (int16_t *)&width, 2);
    memcpy(&buff[10], (int16_t *)&height, 2);
    qDebug() << "--------------------------CameraMount::controlPos" << x << y << width << height<< type;
    sendData(buff, 13);
}

void CameraMount::controlFlow(bool open)
{
    uint8_t buff[5]={0xA5,CAM_DN_FLOW,3,0,0};
    buff[3] = open?1:0; // 1: 打开跟踪  0：关闭跟踪
    qDebug() << "--------------------------CameraMount::controlFlow" << open;
    sendData(buff, 5);
}

void CameraMount::readPIDdata(int state)
{
    uint8_t buff[5]={0xA5,CAM_DN_READPID,3,0,0};
    buff[3] = state; //0:PID写入flash  1:读取PID参数
    qDebug() << "--------------------------CameraMount::readPIDdata" << state;
    sendData(buff, 5);
}

void CameraMount::writePIDdata(int type,int value)
{
    uint8_t buff[7]={0xA5,CAM_DN_CHANGEPID,5,0,0,0,0};
    buff[3] = type; //uint8_t pid_type
    memcpy(&buff[4], &value, 2); //int16_t pid_value
    qDebug() << "--------------------------CameraMount::writePIDdata" << type << value;
    sendData(buff, 7);
}

int CameraMount::switchCameraMode()
{
    static int cameraMode = 0;

    cameraMode = 2 - cameraMode;
    uint8_t buff[5]={0xA5,CAM_DN_CAMMODE,3,0,0};
    buff[3] = cameraMode; //0:录像模式  1:    2：拍照模式
    qDebug() << "--------------------------CameraMount::switchCameraMode" << cameraMode;
    sendData(buff, 5);
    return cameraMode;
}

void CameraMount::switchColor()
{
    _switchColorSumber++;
    _timer->start();
}

void CameraMount::onSwitchColorSend()
{
    uint8_t buff[5]={0xA5,CAM_DN_COLOR_PLATE,3,0,0};
    buff[3] =  _switchColorSumber; //色板跳跃距离 -- 电源板会自动循环
    qDebug() << "--------------------------CameraMount::switchColor" << _switchColorSumber;
    sendData(buff, 5);
    _switchColorSumber = 0;
}

void CameraMount::saveJson(QJsonObject &dataJson)
{
    MountInfo::saveJson(dataJson);
    dataJson.insert("cmdCamType",cam_type());               //云台类型
    dataJson.insert("cmdvideoState",videoStatus());         //录像状态   1：录像中 2：停止录像
    dataJson.insert("cmdModeState",mode());                 //云台模式  0:锁航向  1:自由控 2：一键回中
    dataJson.insert("cmdZoomState",zoom());                 //变焦倍数 [显示当前变焦]
    dataJson.insert("cmdCamPitchAngle",pitchAngle());       //俯仰角度
    dataJson.insert("cmdCamHeadAngle",headAngle());         //偏航角度
    dataJson.insert("cmdCamRoolAngle",rollAngle());         //横滚角度
    dataJson.insert("cmdCamZoom",zoomState());              //0倍数变小  1停止变焦   2倍数变大[操作]
    dataJson.insert("cmdCamErrorCode",errorCode());         //错误代码
    dataJson.insert("cmdCamHasError",hasError());
    dataJson.insert("cmdCamTackPictIndex",tackPictIndex()); //拍照反馈[数量]
}

/* *********************************************************************
 * :解析数据
 * ********************************************************************/
void CameraMount::handleInfo(const can_data &data)
{
    uint8_t tmpData[255]={0};
    //    qDebug()<< "Vehicle::handleCameraInfo"<< data.pack_len;
    for(int i=0; i < data.pack_len; i++)
    {
        if(0 < mountPacket(data.date[i], &tmpData[0]))
        {
//            qDebug() << "------------CameraMount::handleCameraInfo" << tmpData[0];
            switch(tmpData[0]){
            case CameraMount::CAM_UP_CAMINFO:
                parseCamInfo(&tmpData[0], tmpData[1]);
                break;
            case CameraMount::CAM_UP_ANGINFO: //云台角度反馈
                parseCameAngle(&tmpData[0], tmpData[1]);
                break;
            case CameraMount::CAM_UP_ERRINFO:
                parseCamError(&tmpData[0], tmpData[1]);
            case CameraMount::CAM_UP_PHOTOINFO:
                tackPickIndex(&tmpData[0], tmpData[1]);
                break;
            case CameraMount::CAM_UP_ZOOMINFO:
                parseZoomState(&tmpData[0], tmpData[1]);
                break;
            case CameraMount::CAM_UP_PIDINFO:
                parsePID(&tmpData[0], tmpData[1]);
                break;
            case CameraMount::CAM_UP_PIDCHANGE:
                parsePIDWriteRamState(&tmpData[0], tmpData[1]);
                break;
            case CameraMount::CAM_UP_PIDWRITE:
                parsePIDWriteFlashState(&tmpData[0], tmpData[1]);
                break;
            case CameraMount::CAM_UP_TRANSDATA:
                parseBackCode(&tmpData[0], tmpData[1]);
                break;
            case CameraMount::CAM_UP_TEMINFO:
                parseTemCode(&tmpData[0], tmpData[1]);
                break;
            case MOUNT_UPDATA_TYPE::MOUNT_UP_REQUESTTIME:
                sendTime();
                break;
            case MOUNT_UPDATA_TYPE::MOUNT_UP_REQUESTID:
                parseMountIDAck(&tmpData[0], tmpData[1]);
                break;
            }
        }
    }
}

void CameraMount::parseCameAngle(const uint8_t *ptr, int length)
{
    float pitch, yaw, roll;
    if(length != 14)
        return;
    memcpy(&pitch, ptr+2, 4);
    memcpy(&roll, ptr+6, 4);
    memcpy(&yaw, ptr+10, 4);

    //    if(MMCSettings::getvalue("app/enabledFireDetection",false).toBool())
    //    {
    //        gimbal_.setAltitude(roll);       //roll
    //        gimbal_.setLatitude(pitch);       //pitch
    //        gimbal_.setLongitude(yaw);      //yaw
    //        emit gimbalGestureChanged(gimbal_);
    //        _gimbalTimeout->start();
    //        _gimbalLost = false;
    //        emit gimbalLostChanged(_gimbalLost);
    //    }
    if(this->pitchAngle() != pitch)
        this->setPitchAngle(pitch);
    if(this->headAngle() != yaw)
        this->setHeadAngle(yaw);
    if(this->rollAngle() != roll)
        this->setRollAngle(roll);
}

void CameraMount::parseCamError(const uint8_t *ptr, int length)
{

}

void CameraMount::tackPickIndex(const uint8_t *ptr, int length)
{
    if(length != 4)
        return;
    uint16_t index;
    memcpy(&index, ptr+2, 2);
    this->setTackPictIndex(index);
    qDebug() << "------tackPickIndex" << index;
}

void CameraMount::parseBackCode(const uint8_t *ptr, int length)
{
    QByteArray tmp((char *)ptr+2, length-2);
    this->setBackCode(QString(tmp.toHex()));
    qDebug() << "-----parseBackCode" << QString(tmp.toHex());
}

void CameraMount::parseCamInfo(const uint8_t *ptr, int length)
{
    if(length != 8)
        return;
    parseVersion(ptr+2); //版本信息 [字节2,5]
    uint8_t tmpValue;
    memcpy(&tmpValue, ptr+6, 1); //[字节6]
    Camera_Parameter cameradata;
    cameradata.mode = tmpValue & 0x03; //[0-1]
    cameradata.colorPlate = (tmpValue >> 3) & 0x03;    //[3-4]
    cameradata.frame      = (tmpValue >> 5) & 0x01;    //[5]
    cameradata.videoStatu = (tmpValue >> 6) & 0x01;    //[6]
    cameradata.hasError   = (tmpValue >> 7) & 0x01;      //[7]
    cameradata.zoomValue  = *(ptr+7);  //[字节7]
    parseCamParameter(cameradata);
}

void CameraMount::parseCamParameter(const Camera_Parameter& cameradata)
{
    this->setHasError(cameradata.hasError == 1);
    this->setMode(cameradata.mode);
    this->setZoom(cameradata.zoomValue);
    this->setVideoStatus(cameradata.videoStatu);
    //    //飞通用  -- 红外 高清双光
    //    if(camMount->colorPlate() != cameradata.colorPlate)
    //        camMount->setColorPlate(cameradata.colorPlate);
    //    if(camMount->frame() != cameradata.frame)
    //        camMount->setFrame(cameradata.frame);
}

void CameraMount::parseZoomState(const uint8_t *ptr, int length)
{
    if(length != 4 || length != 5 )
        return;
    this->setZoomState(*(ptr+2));
    this->setZoom(*(ptr+3));
    if(5 == length)
        setZoomPosition(*(ptr+4));
}

void CameraMount::parsePID(const uint8_t *ptr, int length)
{
    qDebug()<< "--CameraMount::parsePID" <<length;
    if(length != 62)
        return;
    _dataPIDList.clear();
    _dataPIDListM2.clear();
    _dataPIDListM3.clear();

    emit dataPIDListChanged();
    emit dataPIDListM2Changed();
    emit dataPIDListM3Changed();

    int16_t M1_ATT_KP_AT,M1_ATT_KP_SP,M1_ATT_KI_AT,M1_ATT_KI_SP,M1_ATT_KD_AT,M1_ATT_KD_SP,M1_I_ILIMIT_AT,M1_I_ILIMIT_SP,M1_OUT_ILIMIT_AT,M1_OUT_ILIMIT_SP;
    int16_t M2_ATT_KP_AT,M2_ATT_KP_SP,M2_ATT_KI_AT,M2_ATT_KI_SP,M2_ATT_KD_AT,M2_ATT_KD_SP,M2_I_ILIMIT_AT,M2_I_ILIMIT_SP,M2_OUT_ILIMIT_AT,M2_OUT_ILIMIT_SP;
    int16_t M3_ATT_KP_AT,M3_ATT_KP_SP,M3_ATT_KI_AT,M3_ATT_KI_SP,M3_ATT_KD_AT,M3_ATT_KD_SP,M3_I_ILIMIT_AT,M3_I_ILIMIT_SP,M3_OUT_ILIMIT_AT,M3_OUT_ILIMIT_SP;

    //M1
    memcpy(&M1_ATT_KP_AT, ptr+2, 2);
    memcpy(&M1_ATT_KP_SP, ptr+4, 2);
    memcpy(&M1_ATT_KI_AT, ptr+6, 2);
    memcpy(&M1_ATT_KI_SP, ptr+8, 2);
    memcpy(&M1_ATT_KD_AT, ptr+10, 2);
    memcpy(&M1_ATT_KD_SP, ptr+12, 2);
    memcpy(&M1_I_ILIMIT_AT, ptr+14, 2);
    memcpy(&M1_I_ILIMIT_SP, ptr+16, 2);
    memcpy(&M1_OUT_ILIMIT_AT, ptr+18, 2);
    memcpy(&M1_OUT_ILIMIT_SP, ptr+20, 2);
    //M2
    memcpy(&M2_ATT_KP_AT, ptr+22, 2);
    memcpy(&M2_ATT_KP_SP, ptr+24, 2);
    memcpy(&M2_ATT_KI_AT, ptr+26, 2);
    memcpy(&M2_ATT_KI_SP, ptr+28, 2);
    memcpy(&M2_ATT_KD_AT, ptr+30, 2);
    memcpy(&M2_ATT_KD_SP, ptr+32, 2);
    memcpy(&M2_I_ILIMIT_AT, ptr+34, 2);
    memcpy(&M2_I_ILIMIT_SP, ptr+36, 2);
    memcpy(&M2_OUT_ILIMIT_AT, ptr+38, 2);
    memcpy(&M2_OUT_ILIMIT_SP, ptr+40, 2);
    //M3
    memcpy(&M3_ATT_KP_AT, ptr+42, 2);
    memcpy(&M3_ATT_KP_SP, ptr+44, 2);
    memcpy(&M3_ATT_KI_AT, ptr+46, 2);
    memcpy(&M3_ATT_KI_SP, ptr+48, 2);
    memcpy(&M3_ATT_KD_AT, ptr+50, 2);
    memcpy(&M3_ATT_KD_SP, ptr+52, 2);
    memcpy(&M3_I_ILIMIT_AT, ptr+54, 2);
    memcpy(&M3_I_ILIMIT_SP, ptr+56, 2);
    memcpy(&M3_OUT_ILIMIT_AT, ptr+58, 2);
    memcpy(&M3_OUT_ILIMIT_SP, ptr+60, 2);

    _dataPIDList   << M1_ATT_KP_AT << M1_ATT_KI_AT  << M1_ATT_KD_AT  << M1_I_ILIMIT_AT  << M1_OUT_ILIMIT_AT
                   << M1_ATT_KP_SP << M1_ATT_KI_SP  << M1_ATT_KD_SP  << M1_I_ILIMIT_SP  << M1_OUT_ILIMIT_SP;
    _dataPIDListM2 << M2_ATT_KP_AT << M2_ATT_KI_AT  << M2_ATT_KD_AT  << M2_I_ILIMIT_AT  << M2_OUT_ILIMIT_AT
                   << M2_ATT_KP_SP << M2_ATT_KI_SP  << M2_ATT_KD_SP  << M2_I_ILIMIT_SP  << M2_OUT_ILIMIT_SP;
    _dataPIDListM3 << M3_ATT_KP_AT << M3_ATT_KI_AT  << M3_ATT_KD_AT  << M3_I_ILIMIT_AT  << M3_OUT_ILIMIT_AT
                   << M3_ATT_KP_SP << M3_ATT_KI_SP  << M3_ATT_KD_SP  << M3_I_ILIMIT_SP  << M3_OUT_ILIMIT_SP;

#if 0
    for(int i=0;i<_dataPIDList.count();i++)
    {
        qDebug()<< "--Vehicle::_dataPIDList" <<_dataPIDList.at(i);
    }

    for(int i=0;i<_dataPIDListM2.count();i++)
    {
        qDebug()<< "--Vehicle::_dataPIDListM2" <<_dataPIDListM2.at(i);
    }

    for(int i=0;i<_dataPIDListM3.count();i++)
    {
        qDebug()<< "--Vehicle::_dataPIDListM3" <<_dataPIDListM3.at(i);
    }
#endif
    emit dataPIDListChanged();
    emit dataPIDListM2Changed();
    emit dataPIDListM3Changed();
}

void CameraMount::parsePIDWriteRamState(const uint8_t *ptr, int length)
{
    qDebug()<< "--CameraMount::parsePIDWriteRamState"<< length;
    _writeROMState = false;
    emit writeROMStateChanged();

    if(length != 3)
        return;
    uint8_t tmpValue;
    memcpy(&tmpValue, ptr+2, 1);
    qDebug() << "---------parsePIDWriteRamState" << tmpValue;
    if(tmpValue)
    {
        _writeROMState = true;
        emit writeROMStateChanged();
    }else{
        _writeROMState = false;
        emit writeROMStateChanged();
    }
}

void CameraMount::parsePIDWriteFlashState(const uint8_t *ptr, int length)
{
    qDebug()<< "--CameraMount::parsePIDWriteFlashState"<< length;
    _writeFlashState = false;
    emit writeFlashStateChanged();

    if(length != 3)
        return;
    uint8_t tmpValue;
    memcpy(&tmpValue, ptr+2, 1);
    qDebug() << "---------parsePIDWriteRamState" << tmpValue;
    if(tmpValue)
    {
        _writeFlashState = true;
        emit writeFlashStateChanged();
    }else{
        _writeFlashState = false;
        emit writeFlashStateChanged();
    }
}

