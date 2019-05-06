#ifndef RADIOMEMBER_H
#define RADIOMEMBER_H
/*
 * 电台数据成员类
 */

#include <QObject>
#include "QmlObjectListModel.h"

#include "QGC.h"
#include "radiomemberbase.h"


class RadioMember : public RadioMemberBase
{
    Q_OBJECT

public:

    typedef enum KEY_TYPE
    {
        KEY_ZOOM_DOWN =1,   //缩小
        KEY_ZOOM_UP,        //放大
        KEY_FN,             //FN
        KEY_MODE,           //云台模式
        KEY_REC,            //录像
        KEY_PHOTO,          //拍照

        /* 其它扩展功能 -- 服务自定义功能键 */
        KEY_UNDEFINED,              //未定义
        KEY_VOLUME_UP,              //音量加
        KEY_VOLUME_DOWN,            //音量减
        KEY_BRIGHTNESS_UP,          //亮度加
        KEY_BRIGHTNESS_DOWN,        //亮度减
        KEY_VEHICLE_AUTO_MODE,      //飞机航线模式
        KEY_VEHICLE_LOITER_MODE,    //飞机悬停模式
    } KEY_TYPE;
    explicit RadioMember(QObject *parent = nullptr);


    /* 电台开关状态 */
    Q_PROPERTY(int  radioSourceState        READ radioSourceState               NOTIFY radioSourceStateChanged) //0-关 1-开 2-查询
    /* 自定义功能键界面设置 */
    Q_PROPERTY(QStringList keyModes         READ keyModes                       CONSTANT)
    /* CPU唯一ID -> 作为地面站唯一ID */
    Q_PROPERTY(QString    mmcStationID      READ mmcStationID                   CONSTANT)

    int  radioSourceState   (){return _radioSourceState; }
    QString     mmcStationID(){return _mmcStationID; }

    void setKey(int i, uchar key);
    void setQuantity(uchar value);
    void setVoltage(uchar value);
    void setRadioSourceState(int state);
    void setLidState(int state); //电台盒盖

    /* 按键与功能绑定 */
    void keyBindFunction();
    QStringList keyModes();

    /* 下发操作 */
    Q_INVOKABLE void radioControl(int state); //0-关闭 1-开启
    /* 按键绑定 */
    Q_INVOKABLE int keyBinding(int key, QString type); //return -1_key未找到  -2_type未找到
    Q_INVOKABLE QString getKeyValue(int key); //初始化界面自定义按键选项时用

    void analysisPack(int type, QByteArray msg);
    QString radioSettingUrl(){ return "RadioSettingUrl.qml"; }

signals:
    void radioSourceStateChanged(int state);

public slots:
    void onPress(MMCKey* key);       //按下
    void onUpspring(MMCKey* key);    //弹起
    void onLongPress(MMCKey* key);   //长按
    void onClick(MMCKey* key);       //单击

private:
    void setEnumToStringMapping(const QMap<uint, QString>& enumToString);
    void setVoltage(float value);
    void _say(const QString& text);

    /* 按键触发的的功能方法 */
    void photo();       //拍照
    void REC();         //录像
    void Mode();        //云台模式   1:云台跟随 2：云台回中
    void locking();     //云台模式  0:云台锁定

    void zoomUp();      //放大
    void zoomDown();    //缩小
    void zoomStop();    //停止
    void controlZoom(int zoom); // 0缩小  1停止   2放大
    int  _zoom = 1;

    void fnClick(); //fn功能键 -- 对应挂载

    /* 飞机模式切换 */
    void autoMode();    //航线模式
    void loiterMode();  //悬停模式
    void setVehicleMode(KEY_TYPE keyType);

    //设置系统亮度
    int  setGamma(int direction);   // 负 - 减  正 - 加 [+-125]
    void upGamma();                 //亮度+
    void downGamma();               //亮度-

    //设置软件当前音量
    void setVolume(bool isUp, uchar direction);  // 负 - 减  正 - 加 [+-125]
    void upVolume();                 //音量+
    void downVolume();               //音量-

private:
    /* 获取cpu唯一码 */
    void getCpuId(unsigned int CPUInfo[4], unsigned int InfoType);
    void getCpuIdex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue);
    QString getCpuId();
    QString getCpuId2();
    QString getHostMacAddress();

private:
    /* 按键对应的功能 */
    QMap<int, int>      _keyId;
    /* 8个按键 - 从低位到高位 */
    MMCKey* _key[8];
    /* 电台电源开关 */
    int _radioSourceState = -1;

    /* 软件语言 */
    QString _lang = "";
    QMap<uint, QString> _enumToString;  //自定义功能键

    /* cpu唯一ID */
    QString     _mmcStationID = "";
};

#endif // RADIOMEMBER_H
