#include "custommount.h"
#include "QGCApplication.h"

void MMCChannel::set_value(uint value)
{
    uint tmpValue;
    if(value == _value)
        return;
    if(value < _minValue || value > _maxValue)
        return;

    if(_isPlacement){
        if(value < (_minValue + _maxValue) / 3 ){
            value = _minValue;
        }else if(value < (_minValue + _maxValue) * 2 / 3){
            value = _medValue;
        }else{
            value = _maxValue;
        }
    }else{
        tmpValue = value;
    }

    _value = tmpValue;
    emit valueChanged();

    if(!_isPlacement)//还原
        _timer->start();
}

void MMCChannel::setValue(uint value, uint value2)
{
    set_value(value);
    if(!_isPlacement)//还原
        return;

    _tmp = value2;
    _uiTimer->start();
}

CustomMount::CustomMount(Vehicle *vehicle)
: MountInfo(vehicle)
{
    initChannel();
}

CustomMount::CustomMount(Vehicle *vehicle, int id)
    : MountInfo(vehicle), _readId(id)
{
    initChannel();
}

void CustomMount::initChannel()
{
    for(int i = 0; i < 8; i++){
        MMCChannel* channel = nullptr;

        if(i < 4){
            channel = new MMCChannel(i, false, this);
        }else{
            channel = new MMCChannel(i, true, this);
        }
        connect(channel, &MMCChannel::valueChanged, this, &CustomMount::onChannelChanged);
        _channelList.append(channel);
    }
    _channel1 = dynamic_cast<MMCChannel*>(_channelList.get(0));
    _channel2 = dynamic_cast<MMCChannel*>(_channelList.get(1));
    _channel3 = dynamic_cast<MMCChannel*>(_channelList.get(2));
    _channel4 = dynamic_cast<MMCChannel*>(_channelList.get(3));
    _channel5 = dynamic_cast<MMCChannel*>(_channelList.get(4));
    _channel6 = dynamic_cast<MMCChannel*>(_channelList.get(5));
    _channel7 = dynamic_cast<MMCChannel*>(_channelList.get(6));
    _channel8 = dynamic_cast<MMCChannel*>(_channelList.get(7));

    _timer = new QTimer(this);
    _timer->setInterval(100);
    connect(_timer, &QTimer::timeout, this, &CustomMount::onTimeOut);
    _timer->start();
}

void CustomMount::saveJson(QJsonObject &dataJson)
{
    MountInfo::saveJson(dataJson);
}

void CustomMount::controlPitch(int offset, bool plus)
{
    qDebug() << "------------------------CustomMount::controPich" <<offset << plus;

    if(offset == 0){
        _channel3->set_value(1500);
        return;
    }

    if(plus){
        _channel3->set_value(2030);
    }else{
        _channel3->set_value(1015);
    }
}

void CustomMount::controlHead(int offset, bool plus)
{
    if(offset == 0){
        _channel4->set_value(1500);
        return;
    }

    if(plus){
        _channel4->set_value(1015);
    }else{
        _channel4->set_value(2130);
    }
}

void CustomMount::videoTape()
{
    _channel7->setValue(2130, 1500);
}

void CustomMount::doCameraTrigger()
{
    _channel6->setValue(2130, 1015);
}

void CustomMount::controlZoom(int zoom)
{
    qDebug()<< "-------------------CustomMount::controlZoom" << zoom;
    if(zoom == 0){
        _channel1->set_value(1015);
    }else if(zoom == 2){
        _channel1->set_value(2130);
    }else{
        _channel1->set_value(1500);
    }
}

void CustomMount::virtualMountControl()
{
//    qDebug()<< "-------------------CustomMount::virtualMountControl"
//            << (uint16_t)_channel1->value()
//            << (uint16_t)_channel2->value()
//            << (uint16_t)_channel3->value()
//            << (uint16_t)_channel4->value()
//            << (uint16_t)_channel5->value()
//            << (uint16_t)_channel6->value()
//            << (uint16_t)_channel7->value()
//            << (uint16_t)_channel8->value();

    mavlink_message_t msg;
    mavlink_msg_rc_channels_raw_pack(qgcApp()->toolbox()->mavlinkProtocol()->getSystemId(),
                                     qgcApp()->toolbox()->mavlinkProtocol()->getComponentId(), &msg, 0, 0,
                                     (uint16_t)_channel1->value(),/*roll*/
                                     (uint16_t)_channel2->value(),
                                     (uint16_t)_channel3->value(),/*throtte*/
                                     (uint16_t)_channel4->value(),
                                     (uint16_t)_channel5->value(),
                                     (uint16_t)_channel6->value(),
                                     (uint16_t)_channel7->value(),
                                     (uint16_t)_channel8->value(),
                                     100);
    _vehicle->sendMessageOnLink(_vehicle->priorityLink(), msg);
}

void CustomMount::onChannelChanged()
{
    virtualMountControl();
}

void CustomMount::onTimeOut()
{
    virtualMountControl();
}



