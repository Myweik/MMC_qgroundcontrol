#ifndef PGIY1CAMERAMOUNT_H
#define PGIY1CAMERAMOUNT_H

#include "cameramount.h"
#include <QPoint>
/* 海视英科红外 */
//class QPoint;

class PGIY1CameraMount: public CameraMount
{
    Q_OBJECT
public:
    explicit PGIY1CameraMount(Vehicle* vehicle);

    //海视英科红外
    Q_PROPERTY(float        pointTem        READ pointTem           NOTIFY pointTemChanged)         //指点温度
    Q_PROPERTY(float        minTem          READ minTem             NOTIFY minTemChanged)           //最低温度
    Q_PROPERTY(QPoint       minTemPoint     READ minTemPoint        NOTIFY minTemPointChanged)      //最低温度坐标
    Q_PROPERTY(float        maxTem          READ maxTem             NOTIFY maxTemChanged)           //最高温度
    Q_PROPERTY(QPoint       maxTemPoint     READ maxTemPoint        NOTIFY maxTemPointChanged)      //最高温度坐标
    Q_PROPERTY(float        focusTem        READ focusTem           NOTIFY focusTemChanged)         //中点温度
    Q_PROPERTY(int          colorPlate      READ colorPlate         NOTIFY colorPlateChanged)       //0-4 0：白热 1：红热 2.琥珀 3：黄热

    Q_PROPERTY(float        maxTemThreshold          READ maxTemThreshold     WRITE setMaxTemThreshold        NOTIFY maxTemThresholdChanged)           //最高温度阈值
    Q_PROPERTY(float        minTemThreshold          READ minTemThreshold     WRITE setMinTemThreshold        NOTIFY minTemThresholdChanged)           //最低温度阈值

    float   pointTem        (void){return _pointTem;}                 //指点温度
    float   minTem          (void){return _minTem;}                  //最低温度
    QPoint  minTemPoint     (void){return _minTemPoint;}             //最低温度坐标
    float   maxTem          (void){return _maxTem;}                  //最高温度
    QPoint  maxTemPoint     (void){return _maxTemPoint;}            //最高温度坐标
    float   focusTem        (void){return _focusTem;}               //中心温度
    float   minTemThreshold (void){return _minTemThreshold;}        //最低温度阈值
    float   maxTemThreshold (void){return _maxTemThreshold;}        //最高温度阈值  
    int     colorPlate      (void){return _colorPlate; }            //色板切换

    //色板切换
    Q_INVOKABLE void    controlColorPlate     (int plate);
    void    setCam_type(int type) { Q_UNUSED(type); } //禁用此接口
    void    saveJson        (QJsonObject& dataJson);

private:
    void    setPointTem        (float pointTem);                //指点温度
    void    setMinTem          (float minTem);                  //最低温度
    void    setMinTemPoint     (QPoint minTemPoint);            //最低温度坐标
    void    setMaxTem          (float maxTem);                  //最高温度
    void    serMaxTemPoint     (QPoint maxTemPoint);            //最高温度坐标
    void    setFocusTem        (float focusTem);                //中心温度
    void    setMinTemThreshold (float minTem);                  //最小温度阈值
    void    setMaxTemThreshold (float maxTem);                  //最大温度阈值
    void    setColorPlate       (int plate);                    //色板切换

    void    parseTemCode(const uint8_t* ptr, int length);           // 解析反馈温度信息
    void    parseCamParameter(const Camera_Parameter& cameradata);  //这个函数子类要重写

signals:
    void    maxTemThresholdChanged();
    void    minTemThresholdChanged();
    void    focusTemChanged();
    void    maxTemPointChanged();
    void    maxTemChanged();
    void    minTemPointChanged();
    void    pointTemChanged();
    void    minTemChanged();
    void    colorPlateChanged();

private:
     float      _pointTem = 0.0;                    //指点温度
     float      _minTem = 0.0;                      //最低温度
     QPoint     _minTemPoint;                       //最低温度坐标
     float      _maxTem = 0.0;                      //最高温度
     QPoint     _maxTemPoint;                       //最高温度坐标
     float      _focusTem = 0.0;                    //中心温度

     float      _minTemThreshold = 0;               //最低温度阈值
     float      _maxTemThreshold = 100;             //最高温度阈值
     int        _colorPlate = 0;                    //0-4 0：白热 1：红热 2.琥珀 3：黄热
};

#endif // PGIY1CAMERAMOUNT_H
