#include "androidraduimember.h"
#include <QDebug>

AndroidRaduiMember::AndroidRaduiMember(QObject *parent)
    : RadioMemberBase(parent)
{

}

void AndroidRaduiMember::analysisPack(int type, QByteArray msg)
{
    uchar* buff = (uchar*)msg.data();
    ushort tep = 0;
    float tmp;
    if(type == 0x04)
    qDebug() << "---------------------- AndroidRaduiMember::analysisPack" << type << msg.toHex();
    switch (type) {
    case 0x01:{  //心跳
        this->set_chargeState(*buff++);
        this->set_voltage(*buff++);
        this->set_energy(*buff++);
        this->set_time(((float)*buff++)/10);
        memcpy(&tmp, buff, sizeof(float));
        this->set_temperature(tmp);
        buff += sizeof(float);
        this->set_stateOfHealth(*buff++); 
        this->set_tepyHW(*buff++); //di mian zhan lei xing
        this->set_rcMode(*buff++);
        this->set_calirationState(*buff++);
        this->setVer(buff);
        buff += 4;
        break;
    }
    case 0x03: { //遥控器各通道 -- 16字节
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
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel5(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel6(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel7(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel8(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel9(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel10(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel11(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel12(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel13(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel14(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel15(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channel16(tep);
        buff += 2;
        break;
        }
    case 0x04:{  //遥控器校准时各通道值
        this->set_checkStatus(*buff++);
        break;
    }
    case 0x05:{  //遥控器校准时各通道值
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
        this->set_channelBMid7(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMid8(tep);
        buff += 2;

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
        this->set_channelBMax7(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMax8(tep);
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
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMin7(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMin8(tep);
        buff += 2;

        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBVer1(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBVer2(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBVer3(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBVer4(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBVer7(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBVer8(tep);
        buff += 2;
        break;
    }
    case 0x08:{  //单片机唯一ID
        this->setRadioID(QByteArray((char*)buff, 12));
        break;
    }
    default:
        break;
    }
}
