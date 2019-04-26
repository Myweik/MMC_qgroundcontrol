#ifndef MAPPING3DMOUNT_H
#define MAPPING3DMOUNT_H

#include <QObject>
#include <QFile>
#include <QTimer>
#include "mountinfo.h"
#include "QmlObjectListModel.h"

#include "GasMount/mmcgas.h"

#define  LOOP_STEP 1000
#define  MAX_TEST_NUMBER    50

class Vehicle;
class Mapping3DMount : public MountInfo
{
    Q_OBJECT
public:
    Mapping3DMount(Vehicle* vehicle);
    /* 相机开关状态 */
    Q_PROPERTY(bool                  cam1                      READ cam1         WRITE setCam1          NOTIFY cam1Changed) //相机1开关状态
    Q_PROPERTY(bool                  cam2                      READ cam2         WRITE setCam2          NOTIFY cam2Changed) //相机2开关状态
    Q_PROPERTY(bool                  cam3                      READ cam3         WRITE setCam3          NOTIFY cam3Changed) //相机3开关状态
    Q_PROPERTY(bool                  cam4                      READ cam4         WRITE setCam4          NOTIFY cam4Changed) //相机4开关状态
    Q_PROPERTY(bool                  cam5                      READ cam5         WRITE setCam5          NOTIFY cam5Changed) //相机5开关状态
    /* tf卡识别状态 */
    Q_PROPERTY(bool                  tfCard                    READ tfCard       WRITE setTfCard        NOTIFY tfCardChanged) //TF卡在线状态
    Q_PROPERTY(int                   photoNumber               READ photoNumber  WRITE setPhotoNumber   NOTIFY photoNumberChanged) //拍照数
    Q_PROPERTY(QString               fileName                  READ fileName     WRITE setFileName      NOTIFY fileNameChanged) //POS保存文件名
    Q_PROPERTY(QString               wifiName                  READ wifiName     WRITE setWifiName      NOTIFY wifiNameChanged) //wifi名
    Q_PROPERTY(QString               wifiPassword              READ wifiPassword WRITE setWifiPassword  NOTIFY wifiPasswordChanged) //wifi密码

    Q_INVOKABLE void    switchControl(bool swit); //开关
    Q_INVOKABLE void    getWifi(); //获取无线

    bool    cam1(){return _cam1;}
    bool    cam2(){return _cam2;}
    bool    cam3(){return _cam3;}
    bool    cam4(){return _cam4;}
    bool    cam5(){return _cam5;}
    bool    tfCard(){return _tfCard;}
    int     photoNumber(){return _photoNumber;}
    QString fileName(){return _fileName;}
    QString wifiName(){return _wifiName;}
    QString wifiPassword(){return _wifiPassword;}

    int     mountType(void) const final { return MOUNT_3DMAPPING; }
    void    handleInfo(const can_data& data);
    void    saveJson(QJsonObject& dataJson);

private:
    //云台角度反馈
    void _type0x01(const uint8_t* ptr, int length);
    void _type0x02(const uint8_t* ptr, int length);
    void _type0x03(const uint8_t* ptr, int length);

    void setCam1(bool cam){_cam1 = cam; emit cam1Changed(_cam1);}
    void setCam2(bool cam){_cam2 = cam; emit cam2Changed(_cam2);}
    void setCam3(bool cam){_cam3 = cam; emit cam3Changed(_cam3);}
    void setCam4(bool cam){_cam4 = cam; emit cam4Changed(_cam4);}
    void setCam5(bool cam){_cam5 = cam; emit cam5Changed(_cam5);}
    void setTfCard(bool state){_tfCard = state; emit tfCardChanged(_tfCard);}

    void setPhotoNumber(int number){_photoNumber = number; emit photoNumberChanged(_photoNumber);}
    void setFileName(QString name){_fileName = name; emit fileNameChanged(_fileName);}
    void setWifiName(QString str){_wifiName = str; emit wifiNameChanged(_wifiName);}
    void setWifiPassword(QString str){_wifiPassword = str; emit wifiPasswordChanged(_wifiPassword);}
private:
    int     readId(void)const final  { return mountType(); }
signals:
    void cam1Changed(bool cam);
    void cam2Changed(bool cam);
    void cam3Changed(bool cam);
    void cam4Changed(bool cam);
    void cam5Changed(bool cam);
    void tfCardChanged(bool state);
    void photoNumberChanged(int number);
    void fileNameChanged(QString name);
    void wifiNameChanged(QString name);
    void wifiPasswordChanged(QString name);

private slots:
    void onTimeOut();

private:
    bool _cam1      = false;
    bool _cam2      = false;
    bool _cam3      = false;
    bool _cam4      = false;
    bool _cam5      = false;
    bool _tfCard    = false;

    int _photoNumber = 0;
    QString _fileName = "POS0.txt";
    QString _wifiName = "";
    QString _wifiPassword = "";

    QTimer *timer;
    int _textNumber = 0;

};

#endif // MAPPING3DMOUNT_H
