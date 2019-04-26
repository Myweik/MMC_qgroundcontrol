#ifndef PG2IF1CAMERAMOUNT_H
#define PG2IF1CAMERAMOUNT_H

/* 高清双光 20b */
#include "cameramount.h"

class PG2IF1CameraMount : public CameraMount
{
    Q_OBJECT
public:
    PG2IF1CameraMount(Vehicle* vehicle);

    //双光主画面切换
    Q_PROPERTY(int      frame       READ frame          NOTIFY frameChanged)     //高清双光主画面  0:主画面高清  1:主画面红外
    Q_PROPERTY(int      colorPlate  READ colorPlate     NOTIFY colorPlateChanged)     //红外色板  0:黑白  1:白黑 2：红黄

    int     colorPlate      (void){return _colorPlate; }

    void    setColorPlate   (int plate);
    //高清双光主画面
    Q_INVOKABLE void    controlFrame          (int frame);
    //红外色板切换
    Q_INVOKABLE void    controlColorPlate     (int plate);
    int     frame           (void) { return _frame; }

    void    setCam_type     (int type) { Q_UNUSED(type); } //禁用此接口
    void    saveJson        (QJsonObject& dataJson);
signals:
    void    frameChanged();
    void colorPlateChanged();
private:
    void    setFrame        (int frame);
    void    parseCamParameter(const Camera_Parameter& cameradata);  //这个函数子类要重写
private:
    int     _frame = 0;
    int     _colorPlate = 0;

};

#endif // PG2IF1CAMERAMOUNT_H
