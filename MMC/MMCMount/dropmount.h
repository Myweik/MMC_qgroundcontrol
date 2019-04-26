#ifndef DROPMOUNT_H
#define DROPMOUNT_H

#include <QObject>
#include "mountinfo.h"
class Vehicle;

class DropMount : public MountInfo
{
    Q_OBJECT
public:
    DropMount(Vehicle* vehicle);

    Q_PROPERTY(int currentWeight READ currentWeight WRITE setCurrentWeight NOTIFY currentWeightChanged) //重量  *10g
    Q_PROPERTY(int weight READ weight WRITE setWeight NOTIFY weightChanged) //重量  *10g
    Q_PROPERTY(int state READ state WRITE setState NOTIFY stateChanged)//开闭状态 0：闭合 1：打开
    Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY modeChanged) //0:关闭智能模式   1：开启智能模式

    int weight()        { return _weight;}
    int currentWeight() { return _currentWeight;}
    int state()         { return _state;}
    int mode()          { return _mode;}

    Q_INVOKABLE void dropCmmd();//抛下
    Q_INVOKABLE void controlPitch    (int offset = 5, bool plus=true); //控制俯仰
    Q_INVOKABLE void pitchControl(int offset, bool plus=true); //控制俯仰
    Q_INVOKABLE void weightControl(int weight);//预设重量
    Q_INVOKABLE void modeControl(int mode);//模式控制

    int     mountType(void) const final { return MOUNT_DROP; }
    void    handleInfo(const can_data& data);
    void    saveJson(QJsonObject& dataJson);
private:
    int  readId(void)const final  { return mountType(); }

    void setWeight(int weight);
    void setCurrentWeight(int weight);
    void setState(int state);
    void setMode(int mode);
signals:
    void weightChanged();
    void currentWeightChanged();
    void stateChanged();
    void modeChanged();

private:
    int     _weight;
    int     _currentWeight;
    int     _state;
    int     _mode;
};

#endif // DROPMOUNT_H
