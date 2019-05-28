#include "pg2if1cameramount.h"
#include <QDebug>

PG2IF1CameraMount::PG2IF1CameraMount(Vehicle *vehicle) : CameraMount(vehicle)
{
    CameraMount::setCam_type(CAM_PG2IF1_LS1Z20);
}

void PG2IF1CameraMount::setFrame(int frame)
{
    if(_frame == frame)
        return;
    _frame = frame;
    emit frameChanged();
}

void PG2IF1CameraMount::controlFrame(int frame)
{
    if(frame == _frame)
        return;
    uint8_t buff[5]={0xA5,0x16,3,0,0};
    buff[3] = frame; //0:主画面高清  1:主画面红外
    qDebug() << "--------------------------PG2IF1CameraMount::controlFrame" << frame;
    sendData(buff, 5);
}

void PG2IF1CameraMount::saveJson(QJsonObject &dataJson)
{
    CameraMount::saveJson(dataJson);
    dataJson.insert("cmdCamFrame",frame()); //高清双光主画面  0:主画面高清  1:主画面红外
}

void PG2IF1CameraMount::setColorPlate(int plate)
{
    if(_colorPlate == plate)
        return;
    _colorPlate = plate;
    emit colorPlateChanged();
}

void PG2IF1CameraMount::controlColorPlate(int plate)
{
    if(plate == _colorPlate)
        return;
    uint8_t buff[5]={0xA5,0x15,3,0,0};
    buff[3] = plate; //0:黑白  1:白黑 2：红黄
    qDebug() << "--------------------------PG2IF1CameraMount::controlColorPlate" << plate;
    sendData(buff, 5);
}

void PG2IF1CameraMount::parseCamParameter(const Camera_Parameter &cameradata)
{
    CameraMount::parseCamParameter(cameradata);
    this->setFrame(cameradata.frame);
}
