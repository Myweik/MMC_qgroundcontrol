#ifndef LIGHTMOUNT_H
#define LIGHTMOUNT_H

#include <QObject>
#include "mountinfo.h"
class Vehicle;

class LightMount : public MountInfo
{
    Q_OBJECT
public:

    LightMount(Vehicle* vehicle);

    Q_PROPERTY(int state        READ state      NOTIFY stateChanged)//开闭状态 0：关 1：开
    Q_PROPERTY(float pitchAngle READ pitchAngle NOTIFY pitchAngleChanged) //0:关闭智能模式   1：开启智能模式

    int     state()                 { return _state;}
    float   pitchAngle()            { return _pitchAngle;}

    int     mountType(void) const final { return MOUNT_LIGHT; }
    void    handleInfo(const can_data& data);
    void    saveJson(QJsonObject& dataJson);

    Q_INVOKABLE void lightControl(bool on);//开关
    Q_INVOKABLE void controlPitch    (int offset = 5, bool plus=true); //控制俯仰
    Q_INVOKABLE void pitchControl(int offset, bool plus=true); //控制俯仰
private:
    int     readId(void)const final  { return mountType(); }

    void    setState(int state);
    void    setPitchAngle(float angle);
signals:
    void    stateChanged();
    void    pitchAngleChanged();

private:
    int     _state;
    float   _pitchAngle;
};

#endif // LIGHTMOUNT_H
