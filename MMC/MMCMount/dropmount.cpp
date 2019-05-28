#include "dropmount.h"
#include "QGCApplication.h"
#include "Vehicle.h"

DropMount::DropMount(Vehicle* vehicle)
        : MountInfo(vehicle)
        , _weight(-1)
        , _state(-1)
        , _mode(-1)
        , _currentWeight(-1)
{

}

void DropMount::setWeight(int weight)
{
    if(_weight == weight)
        return;
    _weight = weight;
    emit weightChanged();
}

void DropMount::setState(int state)
{
    if(_state == state)
        return;
    _state = state;
    emit stateChanged();
}

void DropMount::setMode(int mode)
{
    if(mode != 0 && mode != 1)
        return;
    if(_mode == mode)
        return;
    _mode = mode;
    emit modeChanged();
}

void DropMount::setCurrentWeight(int weight)
{
    if(_currentWeight == weight)
        return;
    _currentWeight = weight;
    emit currentWeightChanged();
}

void DropMount::dropCmmd()
{
    uint8_t buff[5] = {0xA5,1,3,1,0};
    qDebug() << "--------------------------DropMount::dropCmmd";
    sendData(buff, buff[2] + 2);
}

void DropMount::controlPitch(int offset, bool plus)
{
    pitchControl(offset, plus);
}

void DropMount::pitchControl(int offset, bool plus)
{
    uint8_t buff[5]={0xA5,5,3,0,0};
    if(!plus)
        buff[3] = -offset;
    else
        buff[3] = offset;
    qDebug() << "--------------------------DropMount::dropCmmd" << offset << plus;
    sendData(buff, buff[2] + 2);
}

void DropMount::weightControl(int weight)
{
    if(_weight == weight)
        return;

    uint8_t buff[6]={0xA5,4,4,0,0,0};
    uint16_t tmpWeight= weight /10;
    memcpy(&buff[3], &tmpWeight, 2);
    qDebug() << "--------------------------DropMount::weightControl" << weight;
    sendData(buff, buff[2] + 2);
}

void DropMount::modeControl(int mode)
{
    if(mode == _mode)
        return;

    uint8_t buff[5]={0xA5,2,3,0,0};
    buff[3] = mode;
    qDebug() << "--------------------------DropMount::modeControl" << mode;
    sendData(buff, buff[2] + 2);
}


void DropMount::handleInfo(const can_data &data)
{
    uint8_t tmpData[255]={0};
    for(int i=0; i < data.pack_len; i++)
    {
        if(0 < mountPacket(data.date[i], &tmpData[0]))
        {
            switch(tmpData[0])
            {
            case 1:
                if(tmpData[1] != 13)
                    return;
                if(1)
                {
                    parseVersion(&tmpData[2]);
                    this->setState(tmpData[7]);
                    uint16_t mess;
                    memcpy(&mess, &tmpData[8], 2);
                    this->setCurrentWeight(mess); //当前挂物重量
                    uint16_t mess2;
                    memcpy(&mess2, &tmpData[10], 2);
                    this->setWeight((int)mess2);//预设最大重量
                    this->setMode((int)tmpData[12]);
                }
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

void DropMount::saveJson(QJsonObject &dataJson)
{
    MountInfo::saveJson(dataJson);
    dataJson.insert("cmdCurrentWeight", currentWeight());   //重量  *10g
    dataJson.insert("cmdWeight", weight());                 //重量  *10g
    dataJson.insert("cmdState", state());                   //开闭状态 0：闭合 1：打开
    dataJson.insert("cmdMode", mode());                     //0:关闭智能模式   1：开启智能模式
}
