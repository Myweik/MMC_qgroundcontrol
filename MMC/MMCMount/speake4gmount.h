#ifndef SPEAKE4GMOUNT_H
#define SPEAKE4GMOUNT_H
#include <QObject>
#include <QFile>
#include "mountinfo.h"
#include "QmlObjectListModel.h"

#include "GasMount/mmcgas.h"
class Vehicle;

#define VOLUME_MIN 1 //最小音量
#define VOLUME_MAX 9 //最大音量
// 4G喊话器
class Speake4GMount : public MountInfo
{
    Q_OBJECT
public:
    enum PlaybackSource{
         SdOrUsb                = 1     //SD卡|USB
        ,TTS                    = 2     //TTS
        ,PoliceSoundPlayback    = 3     //警音播放
        ,ExternalAudio          = 4     //外部音频
        ,DigitalAudio           = 5     //数字音频
    };

    Speake4GMount(Vehicle* vehicle);
    Q_INVOKABLE void srcSelect(int src, int number = 1, int status = 0); /*选择播放源 - 曲目序号 - 播放状态|0 -停止 1 - 播放|*/
    Q_INVOKABLE void volumeUp(); //音量加
    Q_INVOKABLE void volumeDown(); //音量减
    Q_INVOKABLE void volumeChanged(double value); //音量减 value[0.1 - 0.9]
    void volumeControl(int volume);

    Q_INVOKABLE void startPlaying(); //开始播放
    Q_INVOKABLE void stopPlaying(); //停止播放
    Q_INVOKABLE void songUp(); //上一曲
    Q_INVOKABLE void songDown(); //下一曲

    Q_PROPERTY(int  volume              READ volume             WRITE setVolume             NOTIFY volumeChanged)//喊话器当前音量
    Q_PROPERTY(int  playState           READ playState          WRITE setPlayState          NOTIFY playStateChanged) //播放状态
    Q_PROPERTY(int  playSrc             READ playSrc            WRITE setPlaySrc            NOTIFY playSrcChanged) //播放源 //--- 用枚举
    Q_PROPERTY(int  songNumber          READ songNumber         WRITE setSongNumber         NOTIFY songNumberChanged)//喊话器当前曲目
    Q_PROPERTY(bool ttsState            READ ttsState           WRITE setTtsState           NOTIFY ttsStateChanged) //TTS初始化状态
    Q_PROPERTY(bool sdOrUsbState        READ sdOrUsbState       WRITE setSdOrUsbState       NOTIFY sdOrUsbStateChanged)//SD|USB初始化状态
    Q_PROPERTY(bool networkModuleState  READ networkModuleState WRITE setNetworkModuleState NOTIFY networkModuleStateChanged) //公网模块初始化状态

    int  volume()               {return _volume;}
    int  playState()            {return _playState;}
    int  playSrc()              {return _playSrc;}
    int  songNumber()           {return _songNumber;}
    bool ttsState()             {return _ttsState;}
    bool sdOrUsbState()         {return _sdOrUsbState;}
    bool networkModuleState()   {return _networkModuleState;}

    int  mountType(void) const final { return MOUNT_4GSPEAKE; }
    void handleInfo(const can_data& data);
    void saveJson(QJsonObject& dataJson);

private:
    int  readId(void)const final  { return mountType(); }
    void sendData(int len, uint8_t *buf); //发送数据

    void _type0x05(uint8_t* ptr, int length);

    void setVolume(int volume){_volume = volume; emit volumeChanged(_volume);}
    void setPlayState(int playState){_playState = playState; emit playStateChanged(_playState);}
    void setPlaySrc(int playSrc){_playSrc = playSrc; emit playSrcChanged(_playSrc);}
    void setSongNumber(int songNumber){_songNumber = songNumber; emit songNumberChanged(_songNumber);}
    void setTtsState(bool ttsState){_ttsState = ttsState; emit ttsStateChanged(_ttsState);}
    void setSdOrUsbState(bool sdOrUsbState){_sdOrUsbState = sdOrUsbState; emit sdOrUsbStateChanged(_sdOrUsbState);}
    void setNetworkModuleState(bool networkModuleState){_networkModuleState = networkModuleState; emit networkModuleStateChanged(_networkModuleState);}

signals:
    void volumeChanged(int value);
    void playStateChanged (int value);
    void playSrcChanged(int value);
    void songNumberChanged(int value);
    void ttsStateChanged(bool value);
    void sdOrUsbStateChanged(bool value);
    void networkModuleStateChanged(int  value);

private:
    int  _volume = 1;                   //喊话器当前音量
    int  _playState = 0;                //播放状态 0-停止 1-播放
    int  _playSrc = 1;                  //播放源 //--- 用枚举
    int  _songNumber = 1;               //喊话器当前曲目
    bool _ttsState = false;             //TTS初始化状态
    bool _sdOrUsbState = false;         //SD|USB初始化状态
    bool _networkModuleState = false;   //公网模块初始化状态
};

#endif // SPEAKE4GMOUNT_H
