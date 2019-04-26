#ifndef GASMOUNT_H
#define GASMOUNT_H

#include <QObject>
#include <QFile>
#include "mountinfo.h"
#include "QmlObjectListModel.h"

#include "GasMount/mmcgas.h"
class Vehicle;

class GasMount : public MountInfo
{
    Q_OBJECT
public:

    GasMount(Vehicle* vehicle);

    //气体列表
    Q_PROPERTY(QmlObjectListModel* gasList     READ gasList     NOTIFY gasListChanged)
    Q_PROPERTY(int state READ state WRITE setState NOTIFY stateChanged)

    //毒气检测
    QmlObjectListModel* gasList         (void){ return &_gasList; }
    int     state()     { return _state;}

    Q_INVOKABLE void controlPitch    (int offset = 5, bool plus=true); //控制俯仰
    Q_INVOKABLE void pitchControl(int offset, bool plus=true);

    int     mountType(void) const final { return MOUNT_GASDETECTION; }
    void    handleInfo(const can_data& data);
    void    saveJson(QJsonObject& dataJson);

private:
    int     readId(void)const final  { return mountType(); }

    void    setState(int value);
    void    setGasList(QList<gas_info> gasList);

    //处理气体检测心跳包反馈
    void    parseGasInfo(const uint8_t* ptr, int length);
    //处理气体浓度信息反馈
    void    parseConcentration(const uint8_t* ptr, int length);
signals:
    //毒气检测
    void gasListChanged();

    void stateChanged();

private:
    QmlObjectListModel  _gasList;    ///气体列表
    int                 _state;
    QFile*              _gas_file;
};

#endif // GASMOUNT_H
