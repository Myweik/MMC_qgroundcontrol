#include "playvideo.h"
#include <QQmlContext>
#include <QDateTime>
#include <QDebug>
/** 现程实现 */
void AVPlayerTask::run(){
    switch(command){
    case AVPlayerTaskCommand_Render:
        mPlayer->requestRender();
        break;
    case AVPlayerTaskCommand_Seek:
//        mPlayer->seekImpl(param);
        break;
    case AVPlayerTaskCommand_SetPlayRate:
//        mPlayer->slotSetPlayRate(param);
        break;
    }
}

PlayVideo::PlayVideo(QObject *parent) : QObject(parent), mDecoder(new AVDecoder), _rtspPlayer(new RtspPlayer)
{
    mDecoder->setMediaCallback(this);
    wakeupPlayer();

    _rtspPlayer->setDecoder(mDecoder);
    setEncodecStatus(mDecoder->getmIsInitEC());
    connect(mDecoder, &AVDecoder::senderEncodecStatus, this, &PlayVideo::setEncodecStatus);
}

PlayVideo::~PlayVideo()
{
    mThread.clearAllTask();
    delete mDecoder;
    delete _rtspPlayer;
}

void PlayVideo::setUrl(QString url)
{
    if(mDecoder)
        mDecoder->setFilename(url);
}

void PlayVideo::saveTs(bool ok)
{
    if(mDecoder)
        mDecoder->saveTs(ok);
}

void PlayVideo::saveImage()
{
    if(mDecoder)
        mDecoder->saveImage();
}

bool PlayVideo::encodecStatus()
{
    return mEncodecStatus;
}

void PlayVideo::setEncodecStatus(bool state)
{
    mEncodecStatus = state;
    emit encodecStatusChanged();
}

VideoFormat *PlayVideo::getRenderData()
{
     return mRenderData;
}

AVDefine::AVPlayState PlayVideo::getPlaybackState()
{
    return mPlaybackState;
}

void PlayVideo::requestRender()
{
//    qDebug() << "---------------------------" <<" requestRender";
    /* 1-60  >2 45  >3 30  >4 20  >5 15 */
    int len = 0; // mDecoder->getRenderListSize();
    if(len >= 1){
        qint64 lastTime = QDateTime::currentMSecsSinceEpoch();
        qint64 currentTime = mDecoder->requestRenderNextFrame(); //x显示
        int space = QDateTime::currentMSecsSinceEpoch() - lastTime;

        int space2  = _frameStep;
        if(len > _cacheFrame){
            space = 10;
        }else if(len >= _cacheFrame / 2){
            space = space2 - space - _frameStep / 10 -1;;
        }else{
            space = space2 - space + _frameStep / 10 +1;
        }

        if(space <= 0){
            space = 1;
        }else if(space > 45){
            space = 45;
        }

        mMutex.lock();
        mCondition.wait(&mMutex, space);
        mMutex.unlock();

    }
    wakeupPlayer();
}

void PlayVideo::wakeupPlayer()
{
    if(mDecoder){
        mThread.addTask(new AVPlayerTask(this,AVPlayerTask::AVPlayerTaskCommand_Render));
    }
}

/* --------------------------------------[ 继承函数 ]----------------------------------- */
void PlayVideo::mediaUpdateVideoFrame(void *f)
{
    mRenderData = (VideoFormat *)f;
    emit updateVideoFrame(mRenderData);
}

void PlayVideo::mediaStatusChanged(AVDefine::AVMediaStatus)
{

}

void PlayVideo::mediaHasVideoChanged()
{

}

void PlayVideo::mediaUpdateFps(uchar fps)
{
//    qDebug() << "---------------------------mediaUpdateFps fps"  << fps;
    _fps = fps;
    if(_fps != 0 &&_fps <= 80){
        _frameStep = 1000 / _fps + 1;

        _cacheFrame = _cache / _frameStep + 1;
        if(_cacheFrame < 7)
            _cacheFrame = 7;
    }
}

uchar PlayVideo::mediaGetFps()
{
    return _fps;
}
