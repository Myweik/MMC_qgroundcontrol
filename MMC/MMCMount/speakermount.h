#ifndef SPEAKERMOUNT_H
#define SPEAKERMOUNT_H

#include <QObject>
#include "mountinfo.h"
class Vehicle;

class SpeakerMount : public MountInfo
{
    Q_OBJECT
public:

    SpeakerMount(Vehicle* vehicle);

    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)//喊话器当前音量
    Q_PROPERTY(float pitchAngle READ pitchAngle WRITE setPitchAngle NOTIFY pitchAngleChanged)//俯仰角度
    Q_PROPERTY(int state READ state WRITE setState NOTIFY stateChanged) //状态

    int     volume()    { return _volume; }
    int     pitchAngle(){ return _pitchAngle;}
    int     state()     { return _state;}

    int     mountType(void) const final { return MOUNT_SPEAKE; }
    Q_INVOKABLE void volumeControl(int value); //音量控制
    Q_INVOKABLE void controlPitch    (int offset = 5, bool plus=true); //控制俯仰
    Q_INVOKABLE void pitchControl(int offset, bool plus=true);

    void    handleInfo(const can_data& data);
    void    saveJson(QJsonObject& dataJson);

private:
    int     readId(void)const final  { return mountType(); }
    void    setVolume(int value);
    void    setPitchAngle(float value);
    void    setState(int state);

    void    parseSpeakerInfo(const uint8_t* ptr, int length); //处理喊话器角度信息
    void    parseSpeakerAngle(const uint8_t* ptr, int length); //处理喊话器心跳信息

signals:
    void volumeChanged();
    void pitchAngleChanged();
    void stateChanged();

private:
    int     _volume = 0;
    float     _pitchAngle;
    int     _state;

};

#endif // SPEAKERMOUNT_H
