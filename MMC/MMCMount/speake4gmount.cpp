#include "speake4gmount.h"
#include "QGCApplication.h"
#include "Vehicle.h"

Speake4GMount::Speake4GMount(Vehicle *vehicle)
    : MountInfo(vehicle)
{

}

void Speake4GMount::srcSelect(int src, int number, int status)
{
    char len = 0;
    uint8_t buff[255] = {0x00};
    buff[len++] = 0x01; //功能码
    buff[len++] = src; //播放源 1-5
    buff[len++] = status; //播放状态
    buff[len++] = number; //播放曲目
    qDebug() << "--------------------------Speake4GMount::srcSelect" << src << number << status;
    sendData(len, buff);
}

void Speake4GMount::volumeUp()
{
    int volume = _volume + 1;
    if(volume > VOLUME_MAX) volume = VOLUME_MAX;

    volumeControl(volume);
}

void Speake4GMount::volumeDown()
{
    int volume = _volume - 1;
    if(volume < VOLUME_MIN) volume = VOLUME_MIN;

    volumeControl(volume);
}

void Speake4GMount::volumeChanged(double value)
{
    int volume = (int)(value * 10) ;
    if(volume < VOLUME_MIN) volume = VOLUME_MIN;
    if(volume > VOLUME_MAX) volume = VOLUME_MAX;

    volumeControl(volume);
}

void Speake4GMount::volumeControl(int volume)
{
    char len = 0;
    uint8_t buff[255] = {0x00};
    buff[len++] = 0x03; //功能码
    buff[len++] = volume; //data
    qDebug() << "--------------------------Speake4GMount::volumeControl";
    sendData(len, buff);
}

void Speake4GMount::startPlaying()
{
    char len = 0;
    uint8_t buff[255] = {0x00};
    buff[len++] = 0x06; //功能码
    qDebug() << "--------------------------Speake4GMount::startPlaying";
    sendData(len, buff);
}

void Speake4GMount::stopPlaying()
{
    char len = 0;
    uint8_t buff[255] = {0x00};
    buff[len++] = 0x07; //功能码
    qDebug() << "--------------------------Speake4GMount::stopPlaying";
    sendData(len, buff);
}

void Speake4GMount::songUp()
{
    qDebug() << "songUp";
    char len = 0;
    uint8_t buff[255] = {0x00};
    buff[len++] = 0x04; //功能码
    qDebug() << "--------------------------Speake4GMount::songUp";
    sendData(len, buff);
}

void Speake4GMount::songDown()
{
    qDebug() << "songDown";
    char len = 0;
    uint8_t buff[255] = {0x00};
    buff[len++] = 0x05; //功能码
    qDebug() << "--------------------------Speake4GMount::songDown";
    sendData(len, buff);
}

void Speake4GMount::handleInfo(const can_data &data)
{
    uint8_t tmpData[255]={0};
    for(int i=0; i < data.pack_len; i++)
    {
        if(0 < mountPacket(data.date[i], &tmpData[0]))
        {
            switch(tmpData[0])
            {
            case 1:
                parseVersion(&tmpData[2]); //版本信息
                break;
            case 5:
                _type0x05(&tmpData[0], tmpData[1]);
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

void Speake4GMount::saveJson(QJsonObject &dataJson)
{
    MountInfo::saveJson(dataJson);
    dataJson.insert("cmdVolume", volume());                         //喊话器当前音量
    dataJson.insert("cmdPlayState", playState());                   //播放状态
    dataJson.insert("cmdPlaySrc", playSrc());                       //播放源 //--- 用枚举
    dataJson.insert("cmdSongNumber", songNumber());                 //喊话器当前曲目
    dataJson.insert("cmdTtsState", ttsState());                     //TTS初始化状态
    dataJson.insert("cmdSdOrUsbState", sdOrUsbState());             //SD|USB初始化状态
    dataJson.insert("cmdNetworkModuleState", networkModuleState()); //公网模块初始化状态
}

void Speake4GMount::_type0x05(uint8_t *ptr, int length)
{
    uint8_t* buff = ptr+4;
    if( *buff++ == 0x10){ //0x10为供应商设备定义

        int volume = *buff++; //音量
        int songNumber = *buff++; //曲序号
        int playSrec = *buff++; //播放源
        int playStatus = *buff++; //播放状态

        //第十位
        bool SDorUSB =  *buff & (0x01<0); //SD|USB初始化状态
        bool TTS =  *buff & (0x01<1);  //TTS初始化状态
        bool networkModule =  *buff & (0x01<2); //公网模块初始化状态
        this->setVolume(volume);
        this->setPlayState(playStatus);
        this->setPlaySrc(playSrec);
        this->setSongNumber(songNumber);
        this->setTtsState(TTS);
        this->setSdOrUsbState(SDorUSB);
        this->setNetworkModuleState(networkModule);
    }
}

void Speake4GMount::sendData(int len, uint8_t *buf) //发送功能码为01 buf为功能码+数据
{
    uint8_t buff[255] = {0x00}; //申请空间
    uint8_t* data = buff;

    *data++ = 0xa5; //头
    *data++ = 0x05; //功能码
    *data++ = len + 2 + 2; //长度-
    *data++ = (len>>8)&0xff; //长度H
    *data++ = len + 1; //长度L
    for(int i = 0; i < len; i++){ //comand + data
        *data++ = *buf++;
    }

    MountInfo::sendData(buff, buff[2]+2);

//    uint8_t crc = QGC::mount_crc(&buff[1], buff[2]);
//    *data++ = crc; //CRC
//    mavlink_message_t msg;
//    mavlink_mavlink_2_can_t can;
//    can.can_id = writeId();
//    can.valid_len = buff[2]+2;
//    memcpy(can.buf, buff, can.valid_len);
//    mavlink_msg_mavlink_2_can_encode(qgcApp()->toolbox()->mavlinkProtocol()->getSystemId(),
//                                     qgcApp()->toolbox()->mavlinkProtocol()->getComponentId(),
//                                     &msg,
//                                     &can);
//    _vehicle->sendMessageOnLink(_vehicle->priorityLink(), msg);

//    qDebug() << "[" << QByteArray((char*)buff, can.valid_len).toHex() << "]";
}

