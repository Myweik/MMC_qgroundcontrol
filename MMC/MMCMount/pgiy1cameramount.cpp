#include "pgiy1cameramount.h"
#include "QGCApplication.h"

PGIY1CameraMount::PGIY1CameraMount(Vehicle *vehicle) : CameraMount(vehicle)
{
    CameraMount::setCam_type(CAM_PGIY1); //记得调用这个  -- 不然会上传ID不对
//    _minTemThreshold = MMCSettings::dValue("minTemThreshold",0.0,"MOUNT",true).value<float>();
//    _maxTemThreshold = MMCSettings::dValue("maxTemThreshold",0.0,"MOUNT",true).value<float>();
}

void PGIY1CameraMount::controlColorPlate(int plate)
{
    if(plate == _colorPlate)
        return;
    uint8_t buff[5]={0xA5,0x15,3,0,0};
    buff[3] = plate; //0-4 0：白热 1：红热 2.琥珀 3：黄热
    qDebug() << "--------------------------PGIY1CameraMount::controlColorPlate" << plate;
    sendData(buff, 5);
}

void PGIY1CameraMount::saveJson(QJsonObject &dataJson)
{
    CameraMount::saveJson(dataJson);
    dataJson.insert("cmdCamColorPlate",colorPlate());           //色板 0-4 0：白热 1：红热 2.琥珀 3：黄热
    dataJson.insert("cmdCamPointTem",pointTem());               //指点温度
    dataJson.insert("cmdCamMinTem",minTem());                   //最低温度
    dataJson.insert("cmdCamMinTemPoint_X",minTemPoint().x());   //最低温度坐标
    dataJson.insert("cmdCamMinTemPoint_Y",minTemPoint().y());   //最低温度坐标_X
    dataJson.insert("cmdCamMaxTem",maxTem());                   //最高温度
    dataJson.insert("cmdCamMaxTemPoint_X",maxTemPoint().x());   //最高温度坐标_X
    dataJson.insert("cmdCamMaxTemPoint_Y",maxTemPoint().y());   //最高温度坐标_Y
    dataJson.insert("cmdCamFocusTem",focusTem());               //中点温度
}

void PGIY1CameraMount::setPointTem(float pointTem)
{
    if(_pointTem == pointTem)
        return;
    _pointTem = pointTem;
    emit pointTemChanged();
}

void PGIY1CameraMount::setMinTem(float minTem)
{
    if(_minTem == minTem)
        return;
    _minTem = minTem;
    emit minTemChanged();
}

void PGIY1CameraMount::setMinTemPoint(QPoint minTemPoint)
{
    if(_minTemPoint == minTemPoint)
        return;
    _minTemPoint = minTemPoint;
    emit minTemPointChanged();
}

void PGIY1CameraMount::setMaxTem(float maxTem)
{
    if(_maxTem == maxTem)
        return;
    _maxTem = maxTem;
    emit maxTemChanged();
}

void PGIY1CameraMount::serMaxTemPoint(QPoint maxTemPoint)
{
    if(_maxTemPoint == maxTemPoint)
        return;
    _maxTemPoint = maxTemPoint;
    emit maxTemPointChanged();
}

void PGIY1CameraMount::setFocusTem(float focusTem)
{
    if(_focusTem == focusTem)
        return;
    _focusTem = focusTem;
    emit focusTemChanged();
}

void PGIY1CameraMount::setMinTemThreshold(float minTem)
{
    if(_minTemThreshold == minTem)
        return;
    _minTemThreshold = minTem;
    emit minTemThresholdChanged();
//    MMCSettings::dSetValue("minTemThreshold",_minTemThreshold,"MOUNT", true);
    //  _port = MMCSettings::dValue("app/IP Port","","LinkConfigurations",true).value<QString>();
}

void PGIY1CameraMount::setMaxTemThreshold(float maxTem)
{
    if(_maxTemThreshold == maxTem)
        return;
    _maxTemThreshold = maxTem;
    emit maxTemThresholdChanged();
//    MMCSettings::dSetValue("maxTemThreshold",_maxTemThreshold,"MOUNT", true);
}

void PGIY1CameraMount::setColorPlate(int plate)
{
    if(_colorPlate == plate)
        return;
    _colorPlate = plate;
    emit colorPlateChanged();
}

void PGIY1CameraMount::parseTemCode(const uint8_t *ptr, int length)
{
    if(length != 7)
        return;
        int type = *(ptr+2);
        int tem = 0;
        int x = 0;
        int y = 0;
        switch (type) {
        case 0: //指定点温度（小端模式）
            memcpy(&tem, ptr+3, 4);
            this->setPointTem((float)tem/10);
            break;
        case 1: //1：最低温度（小端模式）
            memcpy(&tem, ptr+3, 4);
            this->setMinTem((float)tem/10);
            break;
        case 2: //2:最低温度坐标（byte[4:5]:X,byte[6:7]:Y
            memcpy(&x, ptr+3, 2);
            memcpy(&y, ptr+5, 2);
            this->setMinTemPoint(QPoint(x, y));
            break;
        case 3: //3：最高温度（小端模式）
            memcpy(&tem, ptr+3, 4);
            this->setMaxTem((float)tem/10);
            break;
        case 4: //4:最高温度坐标（与高温类似）
            memcpy(&x, ptr+3, 2);
            memcpy(&y, ptr+5, 2);
            this->setMinTemPoint(QPoint(x, y));
            break;
        case 5: //5:中心点温度
            memcpy(&tem, ptr+3, 4);
            this->setFocusTem((float)tem/10);
            break;
        default:
            break;
        }
}

void PGIY1CameraMount::parseCamParameter(const Camera_Parameter &cameradata)
{
    CameraMount::parseCamParameter(cameradata);
    this->setColorPlate(cameradata.colorPlate);
}

