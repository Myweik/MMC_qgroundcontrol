#ifndef AVDECODER_H
#define AVDECODER_H
#include <QObject>
#include <QReadWriteLock>
#include "AVThread.h"
#include <QVector>
#include <QImage>
#include "AVDefine.h"
#include "AVMediaCallback.h"
#include <QDebug>
#include <QQueue>

#include "usbexample.h"
#include "fifo.h"
//#include <QVideoFrame>

//extern "C"
//{
//#include "libavcodec/avcodec.h"
//#include "libavformat/avformat.h"
//#include "libswscale/swscale.h"
//#include "libavutil/imgutils.h"
//#include <libavformat/avio.h>
//#include <libavutil/file.h>
//}

extern "C"
{

    #ifndef INT64_C
    #define INT64_C
    #define UINT64_C
    #endif

    #include <libavutil/time.h>

    #include <libavcodec/avcodec.h>
    #include <libavcodec/avfft.h>


    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>

    #include <libavfilter/buffersink.h>
    #include <libavfilter/buffersrc.h>
    #include <libavfilter/avfilter.h>

    #include <libavutil/opt.h>

    #include <libswresample/swresample.h>

    #include <libavutil/hwcontext.h>

    #ifdef LIBAVUTIL_VERSION_MAJOR
    #if (LIBAVUTIL_VERSION_MAJOR >= 57)
    #define SUPPORT_HW
    #endif
    #endif
}
class AVDecoder;
class PacketQueue;
class FrameQueue;

class AVDecoder: public QObject
{
    Q_OBJECT
    friend class AVCodecTask;
public:
    explicit AVDecoder(QObject *parent = nullptr);
    ~AVDecoder();
    void setMediaCallback(AVMediaCallback *media);
    bool getmIsInitEC() {return mIsInitEC;}

public :
    qint64 requestRenderNextFrame();
    void load(); //初始化
    void setFilename(const QString &source);
    void rePlay(); //重新加载
    void saveTs(bool status = false);
    void saveImage();

protected:
    void init();
    void init2();
    void getPacket();
    void decodec();
    void setFilenameImpl(const QString &source);
    void _rePlay();

    void requestRender(); //显示数据
    void wakeupPlayer();
signals:
    void frameSizeChanged(int width, int height);
//    void newVideoFrame(const QVideoFrame &frame);
    void senderEncodecStatus(bool);
private:
    usbfifo                 *_usbfifo = nullptr;
    UsbExample              *_hidUSB = nullptr;

//    qint64 lastReadPacktTime = 0;
//    int timeout = 5000;
//    static int interrupt_cb(void *ctx)
//    {
//        AVDecoder* avDecoder = (AVDecoder* )ctx;
//        if(av_gettime() - avDecoder->lastReadPacktTime > avDecoder->timeout * 1000)
//        {
//            return -1;
//        }
//        return 0;
//    }

    /* 推流 */
    void initEncodec();
    int  OpenOutput(string outUrl);

    void getPacketTask(int type = -1);
    void decodecTask(int type = -1);

    void release(bool isDeleted = false);
    void statusChanged(AVDefine::AVMediaStatus);
    int decode_write(AVCodecContext *avctx, AVPacket *packet);
    static enum AVPixelFormat get_hw_format(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts);

private :
    /* fps统计 */
    void onFpsTimeout();
    /* 保存图片 */
    void saveFrame(AVFrame *frame = nullptr);

private:
    QString outUrl;
    bool mIsInitEC = false;
    bool mIsInit = false;
    bool mIsOpenVideoCodec = false;
    QString mFilename; // = "udp://@227.70.80.90:2000";
    int  mVideoIndex;
    AVStream *mVideo = NULL;
    AVFormatContext *mFormatCtx = nullptr;
    AVCodecContext *mVideoCodecCtx = nullptr; //mCodecCtx
    AVCodec *mVideoCodec = nullptr;           //mCodec
    struct SwsContext *mVideoSwsCtx = nullptr; //视频参数转换上下文

    PacketQueue* videoq = nullptr;      //为解码的视频原始包
    FrameQueue* renderq = nullptr;      //为解码后视频包

    /* 资源锁 */
    QReadWriteLock mVideoCodecCtxMutex;

    //  ----- HW
//    QList<AVCodecHWConfig *> mHWConfigList;
    bool                     mUseHw = false;
    /** 硬解格式 */
    enum AVPixelFormat mHWPixFormat;

    /** 保存TS流 */
    FILE *tsSave = nullptr;
    /** 保存Image */
    bool _isSaveImage = false;
    AVFrame *_frameRGB = nullptr;
    uint8_t *_out_buffer = nullptr;
    struct SwsContext *mRGBSwsCtx = nullptr; //RGB转码器 -- 保存图片用

private:
    QMutex mDeleteMutex;

    QTimer      *_fpsTimer = nullptr; //帧率统计心跳
    uint        _fpsFrameSum = 0;

    uchar       _fps = 0;
//    VideoFormat vFormat;
    QSize mSize = QSize(0,0);
    enum AVPixelFormat mPixFormat;  //原始格式格式
    AVThread mProcessThread;
    AVThread mDecodeThread;
    AVThread mPlayeThread; //播放线程
    AVMediaCallback *mCallback = nullptr;
    AVDefine::AVMediaStatus mStatus = AVDefine::AVMediaStatus_UnknownStatus;
};

class AVCodecTask : public Task{
public :
    ~AVCodecTask(){}
    enum AVCodecTaskCommand{
        AVCodecTaskCommand_Init,
        AVCodecTaskCommand_SetPlayRate,
        AVCodecTaskCommand_Seek,
        AVCodecTaskCommand_GetPacket ,
        AVCodecTaskCommand_SetFileName ,
        AVCodecTaskCommand_DecodeToRender,
        AVCodecTaskCommand_SetDecodecMode,
        AVCodecTaskCommand_ShowFrameByPosition,
        AVCodecTaskCommand_RePlay,   //重新加载
        AVCodecTaskCommand_saveTS,   //保存TS流
        AVCodecTaskCommand_saveImage,//保存图片
        AVCoTaskCommand_Render,
    };
    AVCodecTask(AVDecoder *codec,AVCodecTaskCommand command,double param = 0,QString param2 = ""):
        mCodec(codec),command(command),param(param),param2(param2){}
protected :
    /** 现程实现 */
    virtual void run();
private :
    AVDecoder *mCodec;
    AVCodecTaskCommand command;
    double param;
    QString param2;
};

class FrameQueue{
public :
    FrameQueue(){init();}

    QQueue<AVFrame *> frames;
private :
    QReadWriteLock mutex;
public :
    void init(){
        release();
    }

    void put(AVFrame *frame){
        if(frame == NULL)
            return;
        mutex.lockForWrite();
        frames.push_back(frame);
        mutex.unlock();
    }

    AVFrame *get(){
        AVFrame *frame = NULL;
        mutex.lockForWrite();
        if(frames.size() > 0){
            frame = frames.front();
            frames.pop_front();
        }
        mutex.unlock();
        return frame;
    }

    int size(){
        mutex.lockForRead();
        int len = frames.size();
        mutex.unlock();
        return len;
    }

    void release(){
        mutex.lockForWrite();
        QQueue<AVFrame *>::iterator begin = frames.begin();
        QQueue<AVFrame *>::iterator end = frames.end();
        while(begin != end){
            AVFrame *frame = *begin;
            if(frame != NULL){
                av_frame_unref(frame);
                av_free(frame);
            }
            frames.pop_front();
            begin = frames.begin();
        }
        mutex.unlock();
    }
};

class PacketQueue{
public :
    PacketQueue(){init();}

    QQueue<AVPacket *> packets;
//    QList<AVPacket *> packets;
    AVRational time_base;

private :
    QReadWriteLock mutex;
public :

    void setTimeBase(AVRational &timebase){
        this->time_base.den = timebase.den;
        this->time_base.num = timebase.num;
    }

    void init(){
        release();
    }

    void put(AVPacket *pkt){
        if(pkt == NULL)
            return;
        mutex.lockForWrite();
        packets.push_back(pkt);
        mutex.unlock();
    }

    AVPacket *get(){
        AVPacket *pkt = NULL;
        mutex.lockForWrite();
        if(packets.size() > 0){
            pkt = packets.front();
            packets.pop_front();
        }
        mutex.unlock();
        return pkt;
    }

    void removeToTime(int time){
        mutex.lockForRead();
        QQueue<AVPacket *>::iterator begin = packets.begin();
        QQueue<AVPacket *>::iterator end = packets.end();
        while(begin != end){
            AVPacket *pkt = *begin;
            if(pkt != NULL){
                if(av_q2d(time_base) * pkt->pts * 1000 >= time || packets.size() == 1){
                    break;
                }
                av_packet_unref(pkt);
                av_freep(pkt);
            }
            packets.pop_front();
            begin = packets.begin();
        }
        mutex.unlock();
    }

    int diffTime(){
        int time = 0;
        mutex.lockForRead();
        if(packets.size() > 1){
            int start = av_q2d(time_base) * packets.front()->pts * 1000;
            int end = av_q2d(time_base) * packets.back()->pts * 1000;
            time = end - start;
        }
        mutex.unlock();
        return time;
    }

    int startTime(){
        int time = -1;
        mutex.lockForRead();
        if(packets.size() > 0){
            time = av_q2d(time_base) * packets.front()->pts * 1000;
        }
        mutex.unlock();
        return time;
    }

    int size(){
        mutex.lockForRead();
        int len = packets.size();
        mutex.unlock();
        return len;
    }

    void release(){
        mutex.lockForWrite();
        QQueue<AVPacket *>::iterator begin = packets.begin();
        QQueue<AVPacket *>::iterator end = packets.end();
        while(begin != end){
            AVPacket *pkt = *begin;
            if(pkt != NULL){
                av_packet_unref(pkt);
                av_freep(pkt);
            }
            packets.pop_front();
            begin = packets.begin();
        }
        mutex.unlock();
    }
};

#endif // AVDECODER_H
