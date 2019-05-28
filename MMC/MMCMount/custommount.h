#ifndef CUSTOMMOUNT_H
#define CUSTOMMOUNT_H

#include "mountinfo.h"
#include <QTimer>
#include "QGC.h"
#include "QmlObjectListModel.h"
class Vehicle;

class MMCChannel : public QObject
{
    Q_OBJECT
    GENERATE_QML_PROPERTY_ONSET(value, uint, "value")
public:
    explicit MMCChannel(int id, bool isPlacement = false, QObject *parent = nullptr)
        :_id(id), _isPlacement(isPlacement), _value(1523), QObject(parent)
    {
        _timer = new QTimer(this);
        _timer->setInterval(200);
        _timer->setSingleShot(true);
        connect(_timer, &QTimer::timeout, this, &MMCChannel::onTimeOut);

        _uiTimer = new QTimer(this);
        _uiTimer->setInterval(200);
        _uiTimer->setSingleShot(true);
        connect(_uiTimer, &QTimer::timeout, this, &MMCChannel::onUiTimeOut);

    }

    int  id() {return _id; }
    Q_INVOKABLE void setValue(uint value, uint value2 = 1523);

private slots:
    void onTimeOut(){
        set_value(_medValue);
    }

    void onUiTimeOut(){
        set_value(_tmp);
    }

private:
    uint _maxValue = 1015;
    uint _medValue = 1523;
    uint _minValue = 2030;
    int  _id  = 0;
    bool _isPlacement; //确定是否为定值波动
    QTimer* _timer;  //计时器
    QTimer* _uiTimer;  //计时器
    uint _tmp = 1523;
};

class CustomMount : public MountInfo
{
    Q_OBJECT
    /* 通道 */
    GENERATE_QML_PROPERTY(channel1, MMCChannel*, "channel1")
    GENERATE_QML_PROPERTY(channel2, MMCChannel*, "channel2")
    GENERATE_QML_PROPERTY(channel3, MMCChannel*, "channel3")
    GENERATE_QML_PROPERTY(channel4, MMCChannel*, "channel4")
    GENERATE_QML_PROPERTY(channel5, MMCChannel*, "channel5")
    GENERATE_QML_PROPERTY(channel6, MMCChannel*, "channel6")
    GENERATE_QML_PROPERTY(channel7, MMCChannel*, "channel7")
    GENERATE_QML_PROPERTY(channel8, MMCChannel*, "channel8")

public:
    CustomMount(Vehicle* vehicle);
    CustomMount(Vehicle* vehicle, int id);

    int     mountType(void) const final { return MOUNT_CUSTOM; }
    void    handleInfo(const can_data& data){ Q_UNUSED(data); }
    void    saveJson(QJsonObject& dataJson);

    //控制俯仰
    //offset 偏移量  plus偏移方向 true:正方向 false:负方向 up down channel3
    Q_INVOKABLE void    controlPitch    (int offset = 5, bool plus=true);
    //控制偏航
    //offset 偏移量 plus偏移方向 true：正方向 false:负方向 channel4 xiao-you
    Q_INVOKABLE void    controlHead     (int offset = 5, bool plus=true);
    //控制指令
    Q_INVOKABLE void    videoTape       (void); //录像 channel7 3dang zhong-xia(zhixing)
    //控制指令
    Q_INVOKABLE void    doCameraTrigger       (void); //paizhao channel6 2dang shang-xia(zhixing)
    Q_INVOKABLE void    controlZoom     (int zoom); // 0缩小  1停止   2放大  channel1


    //发送坐标点到云台 x,y 坐标点   type坐标类型：1：指点移动  2：指点测温    3：指点跟踪  4.区域测温
    Q_INVOKABLE void    controlPos     (int x, int y, int type=1){}
    //发送矩形到云台 x,y 坐标点  width,height 矩形的宽高   type坐标类型：3：指点跟踪
    Q_INVOKABLE void    controlPos     (int x, int y, int width, int height, int type = 3){}

private:
    int  readId() const final { return _readId; }
    void initChannel();
    void virtualMountControl();

private slots:
    void onChannelChanged();
    void onTimeOut();

private:
    int _readId = 0;
    //激光雷达数据
    QmlObjectListModel _channelList;
    QTimer* _timer;  //计时器
};

#endif // CUSTOMMOUNT_H
