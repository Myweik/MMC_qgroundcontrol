#ifndef PLAYVIDEO_H
#define PLAYVIDEO_H

#include <QObject>
#include <QWaitCondition>
#include "avdecoder.h"
#include <QQmlApplicationEngine>
#include "AVMediaCallback.h"
#include "RtspPlayer.h"

class AVPlayerTask;

class PlayVideo : public QObject , public AVMediaCallback
{
    Q_OBJECT
    friend class AVPlayerTask;
public:
    explicit PlayVideo(QObject *parent = nullptr);
    ~PlayVideo();

    Q_PROPERTY(bool encodecStatus READ encodecStatus /*WRITE setEncodecStatus*/ NOTIFY encodecStatusChanged)
    Q_PROPERTY(RtspPlayer *rtspPlayer READ rtspPlayer)

    Q_INVOKABLE void setUrl(QString url);
    Q_INVOKABLE void saveTs(bool ok = false);
    Q_INVOKABLE void saveImage();

    bool encodecStatus();
    void setEncodecStatus(bool state);

    RtspPlayer * rtspPlayer(){ return _rtspPlayer; }
    VideoFormat *getRenderData();
    AVDefine::AVPlayState getPlaybackState();
protected :
    void requestRender();
public :
    void mediaUpdateVideoFrame(void*);
    void mediaStatusChanged(AVDefine::AVMediaStatus);
    void mediaHasVideoChanged();
    void mediaUpdateFps(uchar fps);
    uchar mediaGetFps();

signals:
    void updateVideoFrame(VideoFormat*);
    void encodecStatusChanged();
public slots:

private:
    void wakeupPlayer();

private:
    uchar       _fps = 0;    //fps
    uchar       _frameStep = 0;

    uint        _cache = 180; //缓存190ms -- 帧数不够时强制缓存数帧
    uchar       _cacheFrame = 0; //根据fps算的缓存帧数

    QWaitCondition mCondition;
    QMutex mMutex;

    AVDecoder *mDecoder;
    AVThread mThread;
    RtspPlayer *_rtspPlayer;

    /** 播放状态 */
    AVDefine::AVPlayState mPlaybackState = AVDefine::AVPlayState_Playing;
    VideoFormat *mRenderData = nullptr;

    bool mEncodecStatus = false;
};

class AVPlayerTask : public Task{
public :
    ~AVPlayerTask(){}
    enum AVPlayerTaskCommand{
        AVPlayerTaskCommand_Render,
        AVPlayerTaskCommand_Seek ,
        AVPlayerTaskCommand_SetPlayRate
    };
    AVPlayerTask(PlayVideo *player,AVPlayerTaskCommand command,double param = 0):
        mPlayer(player),command(command),param(param){this->type = (int)command;}
protected :
    /** 线程实现 */
    virtual void run();
private :
    PlayVideo *mPlayer;
    AVPlayerTaskCommand command;
    double param;
};

#endif // PLAYVIDEO_H
