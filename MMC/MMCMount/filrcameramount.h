#ifndef FILRCAMERAMOUNT_H
#define FILRCAMERAMOUNT_H

#include "cameramount.h"

class FilrCameraMount : public CameraMount
{
    Q_OBJECT
public:
    explicit FilrCameraMount(Vehicle* vehicle);

    //红外 - Filr
    Q_PROPERTY(int      colorPlate  READ colorPlate     NOTIFY colorPlateChanged)     //红外色板  0:黑白  1:白黑 2：红黄

    int     colorPlate      (void){return _colorPlate; }

    void    setColorPlate   (int plate);
    //红外色板切换
    Q_INVOKABLE void    controlColorPlate     (int plate);
    /* 无此类功能  -- 禁用 */
    Q_INVOKABLE void    controlFlow     (bool open) { Q_UNUSED(open); }   //跟踪  -- 并不是所有云台都有此功能

    void    setCam_type     (int type) { Q_UNUSED(type); } //禁用此接口
    void    saveJson(QJsonObject& dataJson);

signals:
    void colorPlateChanged();

public slots:

private:
    void parseCamParameter(const Camera_Parameter& cameradata);  //这个函数子类要重写
private:
    int        _colorPlate = 0;
};

#endif // FILRCAMERAMOUNT_H
