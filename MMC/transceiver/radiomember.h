#ifndef RADIOMEMBER_H
#define RADIOMEMBER_H
/*
 * 电台数据成员类
 */

#include <QObject>
#include "QmlObjectListModel.h"
#include <QTimer>
#include "QGC.h"

class MMCKey : public QObject
{
    Q_OBJECT
public:
    explicit MMCKey(int id, QObject *parent = nullptr);
    Q_PROPERTY(bool key        READ key       NOTIFY keyChanged)

    bool key   (){ return _key; }
    void setKey(bool key);
    int  id() {return _id; }
    double accumulatedTime() { return (double)_accumulatedTime * 0.1; }
signals:
    void press(MMCKey* key);       //按下
    void upspring(MMCKey* key);    //弹起
    void longPress(MMCKey* key);   //长按心跳
    void click(MMCKey* key);       //点击

    void keyChanged(bool key);
private slots:
    void onTimeOut();

private:
    QTimer* timer;  //计时器
    bool _key = false;
    int  _id  = 0;

    int _accumulatedTime = 1;
};

//-------------------------------------------------------------

class RadioMember : public QObject
{
    Q_OBJECT

    /* 充电状态，0为不充电，1为充电中 */
    MMC_GENERATE_QML_PROPERTY(chargeState, uint, "chargeState")
    /* 电池电量剩余时间 */
    MMC_GENERATE_QML_PROPERTY(time, float, "surplusTime")
    /* 电池健康状态 */
    MMC_GENERATE_QML_PROPERTY(stateOfHealth, uint, "stateOfHealth")
    /* 电池温度 */
    MMC_GENERATE_QML_PROPERTY(temperature, float, "temperature")
    /* 遥控器开关状态-遥控器是否把数据发给飞控  0-关，1-开 */
    MMC_GENERATE_QML_PROPERTY(rockerState, uint, "rockerState")
    /* 遥控器模式，左手模式或者右手模式 */
    MMC_GENERATE_QML_PROPERTY(rcMode, uint, "rcMode")
    /* 遥控器校准状态 */
    MMC_GENERATE_QML_PROPERTY(calirationState, uint, "calirationState")
    /* 版本号 */
    MMC_GENERATE_QML_PROPERTY(ver, QString, "Version")
    /* 校准过程状态位置 */
    MMC_GENERATE_QML_PROPERTY(checkStatus, uint, "CheckStatus")

    /* 校准后的通道 1-4 */
    MMC_GENERATE_QML_PROPERTY(channel1, int, "channel1")
    MMC_GENERATE_QML_PROPERTY(channel2, int, "channel2")
    MMC_GENERATE_QML_PROPERTY(channel3, int, "channel3")
    MMC_GENERATE_QML_PROPERTY(channel4, int, "channel4")

    /* 原始通道 1-4 */
    MMC_GENERATE_QML_PROPERTY(channelBMax1, int, "channelBMax1")
    MMC_GENERATE_QML_PROPERTY(channelBMax2, int, "channelBMax2")
    MMC_GENERATE_QML_PROPERTY(channelBMax3, int, "channelBMax3")
    MMC_GENERATE_QML_PROPERTY(channelBMax4, int, "channelBMax4")
    MMC_GENERATE_QML_PROPERTY(channelBMed1, int, "channelBMed1")
    MMC_GENERATE_QML_PROPERTY(channelBMed2, int, "channelBMed2")
    MMC_GENERATE_QML_PROPERTY(channelBMed3, int, "channelBMed3")
    MMC_GENERATE_QML_PROPERTY(channelBMed4, int, "channelBMed4")
    MMC_GENERATE_QML_PROPERTY(channelBMin1, int, "channelBMin1")
    MMC_GENERATE_QML_PROPERTY(channelBMin2, int, "channelBMin2")
    MMC_GENERATE_QML_PROPERTY(channelBMin3, int, "channelBMin3")
    MMC_GENERATE_QML_PROPERTY(channelBMin4, int, "channelBMin4")

public:
        typedef enum Check_Status
        {
            No_Check = 0,                   //未在校准过程
            IN_CALIBRATION = 1,             //校准过程中值——发送
            ACK_IN_CALIBRATION = 2,         //校准过程中值——回收
            GETTING_MAXIMUM_MINIMUM = 3,    //校准过程最值——发送
            ACK_GETTING_MAXIMUM_MINIMUM = 4,//校准过程最值——回收
            CALIBRATION_COMPLETED = 5,      //询问是否完成校准
            ACK_CALIBRATION_COMPLETED = 6,  //回复是否完成校准--已完成
            ERRORS = 0XFF,                  //回复是否完成校准--失败
        } CheckStatus;

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

    /* 串口在线状态 */
    Q_PROPERTY(int  radioLock               READ radioLock                      NOTIFY radioLockChanged) // 0-掉线 1-在线
    /* 电池电量 */
    Q_PROPERTY(int  energy                  READ energy                         NOTIFY energyChanged) //(0, 100]
    /* 电池电压 */
    Q_PROPERTY(float voltage                READ voltage                        NOTIFY voltageChanged) //[13, 16.8]
    /* 电台开关状态 */
    Q_PROPERTY(int  radioSourceState        READ radioSourceState               NOTIFY radioSourceStateChanged) //0-关 1-开 2-查询
    /* 电台唯一ID */
    Q_PROPERTY(QString    radioID           READ radioID                        NOTIFY radioIDChanged)
    /* 自定义功能键界面设置 */
    Q_PROPERTY(QStringList keyModes         READ keyModes                       CONSTANT)
    /* CPU唯一ID -> 作为地面站唯一ID */
    Q_PROPERTY(QString    mmcStationID      READ mmcStationID                   CONSTANT)

    /* url */ //
    Q_PROPERTY(QString    radioSettingUrl   READ radioSettingUrl                   CONSTANT)

    int   radioLock          (){return _radioLock; }
    int   energy             (){return _energy; }
    float voltage            (){return _voltage; }

    int  radioSourceState   (){return _radioSourceState; }
    QString     radioID     (){return _radioID; }
    QString     mmcStationID(){return _mmcStationID; }

    void setKey(int i, uchar key);
    void setQuantity(uchar value);
    void setVoltage(uchar value);
    void setRadioSourceState(int state);
    void setLidState(int state); //电台盒盖
    void setRadioID(QString id);
    void setRadioLock(bool lock);
    /* 设置版本号 */
    void setVer(uchar* buff);

    /* 按键与功能绑定 */
    void keyBindFunction();
    QStringList keyModes();

    /* 下发操作 */
    Q_INVOKABLE void rockerControl(int state); //0-关闭 1-开启
    Q_INVOKABLE void radioControl(int state); //0-关闭 1-开启
    Q_INVOKABLE void sendCheckStatus();       //摇杆校准
     /* 设置遥控器模式 */
    Q_INVOKABLE void setCalirationState(bool isLeft); //true-左手模式 false-右手模式
    /* 按键绑定 */
    Q_INVOKABLE int keyBinding(int key, QString type); //return -1_key未找到  -2_type未找到
    Q_INVOKABLE QString getKeyValue(int key); //初始化界面自定义按键选项时用



    QString radioSettingUrl(){ return "RadioSettingUrl.qml"; }
signals:
    void radioLockChanged(int radioLock);
    void energyChanged(int energy);
    void radioSourceStateChanged(int state);
    void radioIDChanged();
    void voltageChanged();

    void _writeData(char, QByteArray);

public slots:
    void onPress(MMCKey* key);       //按下
    void onUpspring(MMCKey* key);    //弹起
    void onLongPress(MMCKey* key);   //长按
    void onClick(MMCKey* key);       //单击

    void onSyncData();               //后台主动获取数据
private:
    void setEnumToStringMapping(const QMap<uint, QString>& enumToString);
    void setEnergy(int value);
    void setVoltage(float value);
    void _say(const QString& text);

    /* 下发操作 */
    void queryRadioId();          //查询单片机唯一ID

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
    QTimer* _syncDataTimer = nullptr; //从电台同步数据的定时器
    int  _queryNumber = 0;

    /* 按键对应的功能 */
    QMap<int, int>      _keyId;
    /* 8个按键 - 从低位到高位 */
    MMCKey* _key[8];
    /* 电量 */
    int  _energy = 0;
    /* 电压 */
    float _voltage = 0.0; //13-16.8V
    /* 电台电源开关 */
    int _radioSourceState = -1;
    /* 电台ID */
    QString _radioID = "";
    /* 电台在线状态 */
    int _radioLock = 0;

    /* 软件语言 */
    QString _lang = "";
    QMap<uint, QString> _enumToString;  //自定义功能键

    /* cpu唯一ID */
    QString     _mmcStationID = "";
};

#endif // RADIOMEMBER_H
