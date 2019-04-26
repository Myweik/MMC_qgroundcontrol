#include "lightmount.h"
#include "QGCApplication.h"
#include "Vehicle.h"

LightMount::LightMount(Vehicle* vehicle)
        : MountInfo(vehicle)
        , _state(-1)
        , _pitchAngle(0.0)
{

}
void LightMount::setState(int state)
{
    if((state != 0 && state != 1) || _state == state)
        return;
    _state = state;
    emit stateChanged();
}

void LightMount::setPitchAngle(float angle)
{
    if(_pitchAngle == angle)
        return;
    _pitchAngle = angle;
    emit pitchAngleChanged();
}

void LightMount::lightControl(bool on)
{
    uint8_t buff[5] = {0xA5,1,3,0,0};
    if(on)
        buff[3] = 1;
    else
        buff[3] = 0;
    qDebug() << "--------------------------LightMount::lightControl" << on;
    sendData(buff, buff[2] + 2);
}

void LightMount::controlPitch(int offset, bool plus)
{
    pitchControl(offset, plus);
}

void LightMount::pitchControl(int offset, bool plus)
{
    uint8_t buff[5] = {0xA5,6,3,0,0};
    if(plus)
        buff[3] = offset;
    else
        buff[3] = -offset;
    qDebug() << "--------------------------LightMount::pitchControl" << offset << plus;
    sendData(buff, buff[2] + 2);
}

void LightMount::handleInfo(const can_data &data)
{
    uint8_t tmpData[255]={0};
    for(int i=0; i < data.pack_len; i++)
    {
        if(0 < mountPacket(data.date[i], &tmpData[0]))
        {
            switch(tmpData[0])
            {
            case 1:
                parseVersion(&tmpData[2]);
                this->setState(tmpData[6]);
                break;
            case 2:
                float angle;
                memcpy(&angle, &tmpData[2], 4);
                this->setPitchAngle(angle);
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

void LightMount::saveJson(QJsonObject &dataJson)
{
    MountInfo::saveJson(dataJson);
    dataJson.insert("cmdState", state());               //开闭状态 0：关 1：开
    dataJson.insert("cmdPitchAngle", pitchAngle());     //0:关闭智能模式   1：开启智能模式
}
