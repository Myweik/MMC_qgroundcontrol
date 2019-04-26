#ifndef MOUNTINFO_H
#define MOUNTINFO_H

#include <QObject>
#include <QMap>
#include "QGCMAVLink.h"
#include <QJsonObject>

class Vehicle;

typedef struct caninfo{
    uint16_t can_id;
    uint8_t pack_len;
    uint8_t date[8];
} can_data;

class MountInfo : public QObject
{
    Q_OBJECT
public:
    typedef enum MOUNT_TYPE
    {
        MOUNT_CUSTOM=1, //通用挂载[用通道控制]
        MOUNT_SPEAKE=266,//喊话器
        MOUNT_4GSPEAKE=362, //4G喊话器
        MOUNT_LIGHT=282,//探照灯
        MOUNT_DROP=298,//抛投
        MOUNT_GASDETECTION=346,//毒气检测
        MOUNT_CAM_INFO=0, //云台
        MOUNT_HYDROGNE=300,
        MOUNT_FOC=316,
        MOUNT_3DMAPPING=284, //倾斜摄影
        MOUNT_INDICATORLIGHT=268,
    } MOUNT_TYPE;

    // 挂载上传上行指令
    typedef enum MOUNT_UPDATA_TYPE
    {
        MOUNT_UP_REQUESTTIME=11,  //请求时间
        MOUNT_UP_REQUESTID=0xFE,  //请求ID
    } MOUNT_UPDATA_TYPE;

    explicit MountInfo(Vehicle* vehicle, QObject* parent = 0);
    Q_PROPERTY(QString version          READ version                    NOTIFY versionChanged)      //版本号
    Q_PROPERTY(int     mountType        READ mountType                  NOTIFY mountTypeChanged)    //挂载类型
    Q_PROPERTY(QString mountID          READ mountID                    NOTIFY mountIDChanged)      //挂载唯一ID

    Q_INVOKABLE QString mountName(int mountId) { (void)mountId;  return _idName[readId()]; }
    Q_INVOKABLE QString mountMode(int mountId) { (void)mountId;  return _idString[readId()]; }

    Q_INVOKABLE void controlCommand(QString code); //透传

    QString      mountID()   { return _mountID;}
    QString      version()   { return _version;}
    int          writeId()   { return _canId[readId()];}

    virtual void handleInfo(const can_data& data) = 0; //解包函数
    virtual int  mountType  (void) const = 0;
    virtual void saveJson(QJsonObject& dataJson);   //转JSON

private:
    void         setMountID(uint32_t* mountId);

    void         requestMountID();  //获取ID
    virtual int  readId     (void) const = 0;
    void         setVersion(QString version);

signals:
    void    versionChanged();
    void    mountTypeChanged();
    void    mountIDChanged();

public slots:

protected:
    void    parseMountIDAck(const uint8_t* ptr, int length); //解析ID与版本号
    void    parseVersion(const uint8_t* p); //处理版本信息
    int     mountPacket(const uint8_t data, uint8_t *dst); //return length 长度 -1未完成组包  >0组包完成
    bool    _packeting = false;
    QList<uint8_t> _packList;

    void    sendData(uint8_t *buf, int len); //发送数据  --- 此函数计算crc 传入时crc位置可以为任意值（len包含crc位置）
    void    sendTime();
protected:
    Vehicle*            _vehicle;

private:
    QMap<int, int>      _canId;
    QMap<int, QString>  _idString;
    QMap<int, QString>  _idName;
    int                 _mountId;
    QString             _version;

    bool                _hasMountId = false;            //是否获取到挂载的id
    uint32_t            _mountRequestId[4];             //挂载的id
    QString             _mountID = "...";               //挂载的id
};

#endif // MOUNTINFO_H
