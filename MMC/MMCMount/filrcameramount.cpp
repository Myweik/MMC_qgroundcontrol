#include "filrcameramount.h"
#include "cameramount.h"

FilrCameraMount::FilrCameraMount(Vehicle *vehicle): CameraMount(vehicle)
{
    CameraMount::setCam_type(CAM_Filr);
}

void FilrCameraMount::setColorPlate(int plate)
{
    if(_colorPlate == plate)
        return;
    _colorPlate = plate;
    emit colorPlateChanged();
}

void FilrCameraMount::controlColorPlate(int plate)
{
    if(plate == _colorPlate)
        return;
    uint8_t buff[5]={0xA5,0x15,3,0,0};
    buff[3] = plate; //0:黑白  1:白黑 2：红黄
    qDebug() << "--------------------------FilrCameraMount::controlColorPlate" << plate;
    sendData(buff, 5);
}

void FilrCameraMount::saveJson(QJsonObject &dataJson)
{
    CameraMount::saveJson(dataJson);
    dataJson.insert("cmdCamColorPlate",colorPlate());           //色板 0:黑白  1:白黑 2：红黄
}

void FilrCameraMount::parseCamParameter(const Camera_Parameter &cameradata)
{
//    qDebug() << "---------------------parseCamParameter" << cameradata.colorPlate;
    CameraMount::parseCamParameter(cameradata);
    this->setColorPlate(cameradata.colorPlate);
}
