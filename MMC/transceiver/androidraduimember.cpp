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
//    if(type == 0x03)
//    qDebug() << "---------------------- AndroidRaduiMember::analysisPack" << type << msg.toHex();
    switch (type) {
    case 0x01: { //遥控器各通道 -- 16字节
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
//            this->setKey(i, *buff++);
        }
        break;
        }
        //73 39 00 05 01 83 26 02 82 2c 00 00 33 93 88 c3
    case 0x03:{  //心跳
        this->set_chargeState(*buff++);
        this->set_voltage(*buff++);
        this->set_energy(*buff++);
        this->set_time(((float)*buff++)/10);
        this->set_stateOfHealth(*buff++);
        memcpy(&tmp, buff, sizeof(float));
        this->set_temperature(tmp);
        buff += sizeof(float);
        this->set_rockerState((*buff) & 0x01);
//        this->setRadioSourceState(((*buff)>>1 & 0x01));
//        this->setLidState(((*buff)>>2 & 0x01));
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
        this->set_channelBMed1(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMed2(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMed3(tep);
        buff += 2;
        memcpy(&tep, buff, sizeof(ushort));
        this->set_channelBMed4(tep);
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
