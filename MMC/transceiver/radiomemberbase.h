#ifndef RADIOMEMBERBASE_H
#define RADIOMEMBERBASE_H

#include <QObject>
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
class RadioMemberBase : public QObject
{
    Q_OBJECT

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

    /* 校准过程状态位置 */
    MMC_GENERATE_QML_PROPERTY(checkStatus, uint, "CheckStatus")

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

    /* 电池电量 */
    MMC_GENERATE_QML_PROPERTY(energy, uint, "energy")
    /* 电池电压 */
    MMC_GENERATE_QML_PROPERTY(voltage, float, "voltage")
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
    explicit RadioMemberBase(QObject *parent = nullptr);

    /* url */ //
    Q_PROPERTY(QString    radioSettingUrl   READ radioSettingUrl                   CONSTANT)
    /* 串口在线状态 */
    Q_PROPERTY(int  radioLock               READ radioLock                      NOTIFY radioLockChanged) // 0-掉线 1-在线
    /* 电台唯一ID */
    Q_PROPERTY(QString    radioID           READ radioID                        NOTIFY radioIDChanged)
public:
    virtual QString radioSettingUrl() = 0;
    virtual void analysisPack(int type, QByteArray msg) = 0;

    int   radioLock  (){return _radioLock; }
    void  setRadioLock(bool lock);
    QString radioID  (){return _radioID; }
    void setRadioID(QString id);
    /* 设置版本号 */
    void setVer(uchar* buff);

    /* 下发操作 */
    void queryRadioId();          //查询单片机唯一ID
    /* 设置遥控器模式 */
    Q_INVOKABLE void setCalirationState(bool isLeft); //true-左手模式 false-右手模式
    Q_INVOKABLE void sendCheckStatus();       //摇杆校准
    Q_INVOKABLE void rockerControl(int state); //0-关闭 1-开启
signals:
    void _writeData(char, QByteArray);
    void radioLockChanged();
    void radioIDChanged();

public slots:
     void onSyncData();               //后台主动获取数据

private:
        QTimer* _syncDataTimer = nullptr; //从电台同步数据的定时器

        /* 电台在线状态 */
        int _radioLock = 0;
        /* 电台ID */
        QString _radioID = "";
};

#endif // RADIOMEMBERBASE_H
