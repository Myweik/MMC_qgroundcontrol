#include "speakermount.h"
#include "QGCApplication.h"
#include "Vehicle.h"

SpeakerMount::SpeakerMount(Vehicle* vehicle)
        : MountInfo(vehicle)
        , _volume(0)
        , _pitchAngle(0)
{

}

void SpeakerMount::setVolume(int value)
{
    if(_volume == value)
        return;
    _volume = value;
    emit volumeChanged();
}

void SpeakerMount::setPitchAngle(float value)
{
    if(_pitchAngle == value)
        return;
    _pitchAngle = value;
    emit pitchAngleChanged();
}

void SpeakerMount::setState(int state)
{
    if(_state == state)
        return;
    _state = state;
    emit stateChanged();
}

void SpeakerMount::volumeControl(int value)
{
    if(value == _volume || value < 200 || value > 230)
        return;
    uint8_t buff[5] = {0xA5,2,3,0,0};
    buff[3] = value;
    qDebug() << "--------------------------SpeakerMount::volumeControl";
    sendData(buff, buff[2] + 2);
}

void SpeakerMount::controlPitch(int offset, bool plus)
{
    pitchControl(offset, plus);
}

void SpeakerMount::pitchControl(int offset, bool plus)
{
    uint8_t buff[5] = {0xA5,4,3,0,0};
    if(plus)
        buff[3] = offset;
    else
        buff[3] = -offset;
    qDebug() << "--------------------------SpeakerMount::pitchControl";
    sendData(buff, buff[2] + 2);
}

void SpeakerMount::handleInfo(const can_data &data)
{
    uint8_t tmpData[255]={0};
    for(int i=0; i < data.pack_len; i++)
    {
        if(0 < mountPacket(data.date[i], &tmpData[0]))
        {
            switch(tmpData[0])
            {
            case 1:
                parseSpeakerInfo(&tmpData[0], tmpData[1]);
                break;
            case 2:
                parseSpeakerAngle(&tmpData[0], tmpData[1]);
                break;
            case 3:
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

void SpeakerMount::saveJson(QJsonObject &dataJson)
{
    MountInfo::saveJson(dataJson);

    dataJson.insert("cmdVolume", volume());         //喊话器当前音量
    dataJson.insert("cmdPitchAngle", pitchAngle()); //俯仰角度
    dataJson.insert("cmdState", state());           //状态
}

void SpeakerMount::parseSpeakerInfo(const uint8_t *ptr, int length)
{
    if(length != 8)
            return;
    parseVersion(ptr+2); //版本信息
    this->setVolume((int)*(ptr+6));
}

void SpeakerMount::parseSpeakerAngle(const uint8_t *ptr, int length)
{
    if(length != 6)
        return;
    float pitchAngle;
    memcpy(&pitchAngle, ptr+2, 4);
    this->setPitchAngle(pitchAngle);
}
