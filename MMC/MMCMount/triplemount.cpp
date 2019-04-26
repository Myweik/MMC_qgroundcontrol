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
//    mavlink_message_t msg;
//    mavlink_mavlink_2_can_t can;

//    can.can_id = writeId(mountType());

    uint8_t buff[5] = {0xA5,1,3,1,0};
    qDebug() << " -----------------dropCmmd";
    sendData(buff, 5);

//    buff[4] = QGC::mount_crc(&buff[1], 3);
//    memcpy(can.buf, buff, 5);
//    mavlink_msg_mavlink_2_can_encode(qgcApp()->toolbox()->mavlinkProtocol()->getSystemId(),
//                                     qgcApp()->toolbox()->mavlinkProtocol()->getComponentId(),
//                                     &msg,
//                                     &can);
//    qDebug() << " -----------------dropCmmd"<< can.can_id << can.buf[0] << can.buf[1];
//    _vehicle->sendMessageOnLink(_vehicle->priorityLink(), msg);
}

void DropMount::pitchControl(int offset, bool plus)
{
    uint8_t buff[5]={0xA5,5,3,0,0};
    if(!plus)
        buff[3] = -offset;
    else
        buff[3] = offset;
    qDebug() << " -----------------pitchControl";
    sendData(buff, 5);
}

void DropMount::weightControl(int weight)
{
    if(_weight == weight)
        return;
    uint8_t buff[6]={0xA5,4,4,0,0,0};
    uint16_t tmpWeight= weight /10;
    memcpy(&buff[3], &tmpWeight, 2);
    qDebug() << " -----------------weightControl";
    sendData(buff, 6);
}

void DropMount::modeControl(int mode)
{
    if(mode == _mode)
        return;
    uint8_t buff[5]={0xA5,2,3,0,0};
    buff[3] = mode;
    qDebug() << " -----------------modeControl";
    sendData(buff, 5);
}
