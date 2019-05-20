#include "avdecoder.h"
#include <QDebug>
#include "VideoBuffer.h"

#include <QDateTime>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>

#include <QProcess>

#include "painter/GlPainter.h"

#define HW true

//#include "Settings/mmcsettings.h"
//输出的地址
AVFormatContext *outputContext = nullptr;
AVFormatContext *inputContext = nullptr;

AVPixelFormat AVDecoder::get_hw_format(AVCodecContext *ctx, const AVPixelFormat *pix_fmts)
{

    AVDecoder *opaque = (AVDecoder *) ctx->opaque;
    const enum AVPixelFormat *p;
    for (p = pix_fmts; *p != -1; p++) {
        if (*p == opaque->mHWPixFormat)
        {
            qDebug() << (*p) << ";" << opaque->mHWPixFormat;
            qDebug() << "Successed to get HW surface format.";
            return *p;
        }
    }
    qDebug() << "Failed to get HW surface format.";
    return AV_PIX_FMT_NONE;
}

static int lockmgr(void **mtx, enum AVLockOp op)
{
    switch(op) {
    case AV_LOCK_CREATE:{
        QMutex *mutex = new QMutex;
        *mtx = mutex;
        return 0;
    }
    case AV_LOCK_OBTAIN:{
        QMutex *mutex = (QMutex*)*mtx;
        mutex->lock();
        return 0;
    }
    case AV_LOCK_RELEASE:{
        QMutex *mutex = (QMutex*)*mtx;
        mutex->unlock();
        return 0;
    }
    case AV_LOCK_DESTROY:{
        QMutex *mutex = (QMutex*)*mtx;
        delete mutex;
        return 0;
    }
    }
    return 1;
}

void AVCodecTask::run(){
    switch(command){
    case AVCodecTaskCommand_Init:
        mCodec->init2();
        break;
    case AVCodecTaskCommand_SetPlayRate: //播放速率
        break;
    case AVCodecTaskCommand_Seek:        //跳转
        break;
    case AVCodecTaskCommand_GetPacket:     //获取数据
        mCodec->getPacket();
        break;
    case AVCodecTaskCommand_SetFileName: //设置播放对象 -- url
        mCodec->setFilenameImpl(param2);
        break;
    case AVCodecTaskCommand_DecodeToRender : //+ 渲染成帧
        mCodec->decodec();
        break;
    case AVCodecTaskCommand_SetDecodecMode :
        break;
    case AVCodecTaskCommand_ShowFrameByPosition :   //按位置显示帧
        break;
    case AVCodecTaskCommand_RePlay :   //重新加载
        mCodec->_rePlay();
        break;

    case AVCoTaskCommand_Render :   //播放
        mCodec->requestRender();
        break;

    }
}

AVDecoder::AVDecoder(QObject *parent) : QObject(parent), videoq(new PacketQueue), renderq(new FrameQueue)
{
#ifdef LIBAVUTIL_VERSION_MAJOR
#if (LIBAVUTIL_VERSION_MAJOR < 56)
    avcodec_register_all();
    avfilter_register_all();
    av_register_all();
    avformat_network_init();
#endif
#endif

    avcodec_register_all();
    avfilter_register_all();
    av_register_all();
    avformat_network_init();

    av_log_set_callback(NULL);//不打印日志
    av_lockmgr_register(lockmgr);
    outUrl = ""; // "rtmp://192.168.5.138:1936/live/test";
    //    outUrl = MMCSettings::getvalue("video/rtmp/videoUrl", "").toString();

    _fpsTimer = new QTimer(this);
    _fpsTimer->setInterval(1000);
    connect(_fpsTimer, &QTimer::timeout, this, &AVDecoder::onFpsTimeout);
    _fpsTimer->start();

    _usbfifo = new usbfifo();
//    _usbfifo->initialfifo();
    _hidUSB = new UsbExample(this);

    videoq->release();
    renderq->release();

    wakeupPlayer(); //启动显示线程
}

AVDecoder::~AVDecoder()
{
    QMutexLocker locker(&mDeleteMutex);
    _fpsTimer->stop();
    mProcessThread.stop();
    mDecodeThread.stop();
    mPlayeThread.stop();
    int i = 0;
    while(!mProcessThread.isRunning() && i++ < 200){
        QThread::msleep(1);
    }
    i = 0;
    while(!mDecodeThread.isRunning() && i++ < 200){
        QThread::msleep(1);
    }
    i = 0;
    while(!mPlayeThread.isRunning() && i++ < 200){
        QThread::msleep(1);
    }

    release(true);
    if(videoq){
        videoq->release();
        delete videoq;
        videoq =nullptr;
    }

    if(renderq){
        renderq->release();
        delete renderq;
        renderq =nullptr;
    }

}

void AVDecoder::setMediaCallback(AVMediaCallback *media){
    this->mCallback = media;
}

/* ---------------------------线程任务操作-------------------------------------- */
void AVDecoder::load(){
    mProcessThread.addTask(new AVCodecTask(this,AVCodecTask::AVCodecTaskCommand_Init));
}

void AVDecoder::setFilename(const QString &source){
    mProcessThread.addTask(new AVCodecTask(this,AVCodecTask::AVCodecTaskCommand_SetFileName,0,source));
}

void AVDecoder::rePlay()
{
    mProcessThread.addTask(new AVCodecTask(this,AVCodecTask::AVCodecTaskCommand_RePlay));
}

void AVDecoder::wakeupPlayer()
{
    mPlayeThread /*mProcessThread*/.addTask(new AVCodecTask(this, AVCodecTask::AVCoTaskCommand_Render));
}

void AVDecoder::saveTs(bool status)
{
    static bool cstatus = !status;
    if(cstatus == status){
        return;
    }else{
        cstatus = status;
    }
    if(status){
        if(tsSave == nullptr){
            QString dir = QString("%1").arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + "/MMC Station/Media/Video/" + QDateTime::currentDateTime().toString("yyyyMMdd") + "/";
            QDir tmpDir1;
            qDebug() << "---------------------dir" << dir;
            bool ok = false;
            if(tmpDir1.exists(dir)){
                ok = true;
            }else{
                ok = tmpDir1.mkpath(dir);
            }
            if(ok){
                QString fileName = QDateTime::currentDateTime().toString("hh_mm_ss");
                tsSave = fopen(QString("%1%2%3").arg(dir).arg(fileName).arg(".ts").toStdString().data(), "wb");
            }
        }
    }else{
        if(tsSave){
            fclose(tsSave);
            tsSave = nullptr;
        }
    }
}

void AVDecoder::saveImage()
{
    _isSaveImage = true;
}

void AVDecoder::getPacketTask(int type)
{
    mProcessThread.addTask(new AVCodecTask(this,AVCodecTask::AVCodecTaskCommand_GetPacket));
}

void AVDecoder::decodecTask(int type)
{
   /* mDecodeThread */mProcessThread.addTask(new AVCodecTask(this,AVCodecTask::AVCodecTaskCommand_DecodeToRender));
}

//static int hw_decoder_init(AVCodecContext *ctx, AVBufferRef *hw_device_ctx, const AVCodecHWConfig* config)
//{
//    int err = 0;
//    if ((err = av_hwdevice_ctx_create(&hw_device_ctx, config->device_type,
//                                      NULL, NULL, 0)) < 0) {
//        fprintf(stderr, "Failed to create specified HW device.\n");
//        return err;
//    }
//    ctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);
//    av_buffer_unref(&hw_device_ctx);
//    hw_device_ctx = nullptr;
//    ctx->pix_fmt = config->pix_fmt;
//    return err;
//}

void AVDecoder::init(){
    return;

    QMutexLocker locker(&mDeleteMutex);
    if(mIsInit)
        return;
    mIsInit = true;
    statusChanged(AVDefine::AVMediaStatus_Loading);

    /* ----------------------------只是打印出来看看--------------------------- */
    //    static QStringList codecs;
    //    AVCodec* c = NULL;
    //    while ((c=av_codec_next(c))) {
    //        if (!av_codec_is_decoder(c) || c->type != AVMEDIA_TYPE_VIDEO)
    //            continue;
    //        codecs.append(QString::fromLatin1(c->name));
    //    }
    //    qDebug() << "-----------------------codecs.size" << codecs.size() << codecs;

    AVDictionary* options = NULL;
    //    av_dict_set(&options, "buffer_size", "102400", 0);  //增大“buffer_size”
    //    av_dict_set(&options, "max_delay", "500000", 0);
    //    av_dict_set(&options, "stimeout", "20000000", 0);  //设置超时断开连接时间
    av_dict_set(&options, "preset", "ultrafast ", 0); // av_opt_set(pCodecCtx->priv_data,"preset","fast",0);
    av_dict_set(&options, "tune", "zerolatency", 0);
    av_dict_set(&options, "rtsp_transport", "udp", 0);  //以udp方式打开，如果以tcp方式打开将udp替换为tcp
    //寻找视频
    mFormatCtx = avformat_alloc_context();
//    lastReadPacktTime = av_gettime();
    mFormatCtx->interrupt_callback.opaque = this;
//    mFormatCtx->interrupt_callback.callback = interrupt_cb;

    if(avformat_open_input(&mFormatCtx, mFilename.toStdString().c_str(), NULL, &options) != 0) //打开视频
    {
        qDebug() << "media open error : " << mFilename.toStdString().data();
        statusChanged(AVDefine::AVMediaStatus_NoMedia);
        mIsInit = false;
        QThread::msleep(10);
        rePlay();
        return;
    }
    mIsInit = false;

    //    mFormatCtx->probesize = 50000000;  //
    //    qDebug() << "------------mFormatCtx->probesize  mFormatCtx->max_analyze_duration2"<< mFormatCtx->probesize << mFormatCtx->max_analyze_duration;
    if(avformat_find_stream_info(mFormatCtx, NULL) < 0) //判断是否找到视频流信息
    {
        qDebug() << "media find stream error : " << mFilename.toStdString().data();
        statusChanged(AVDefine::AVMediaStatus_InvalidMedia);

        QThread::msleep(10);
        rePlay();
        return;
    }

    AVCodecParameters *pCodecPar = mFormatCtx->streams[AVMEDIA_TYPE_VIDEO]->codecpar; //查找解码器
    //获取一个合适的编码器pCodec find a decoder for the video stream
    // AVCodec *pCodec = avcodec_find_decoder(pCodecPar->codec_id);
    AVCodec *pCodec;
    switch (pCodecPar->codec_id){
    case AV_CODEC_ID_H264:
        pCodec = avcodec_find_decoder_by_name("h264_mediacodec");//硬解码264
        if (pCodec == NULL) {

            qDebug("Couldn't find Codec.\n");
//            return -1;
        }
        break;
    case AV_CODEC_ID_MPEG4:
        pCodec = avcodec_find_decoder_by_name("mpeg4_mediacodec");//硬解码mpeg4
        if (pCodec == NULL) {

            qDebug("Couldn't find Codec.\n");
//            return -1;
        } break;
    case AV_CODEC_ID_HEVC:
        pCodec = avcodec_find_decoder_by_name("hevc_mediacodec");//硬解码265
        if (pCodec == NULL) {
            qDebug("Couldn't find Codec.\n");
//            return -1;
        }
        break;
    default:
        pCodec = avcodec_find_decoder(pCodecPar->codec_id);//软解
        if (pCodec == NULL) {
            qDebug("Couldn't find Codec.\n");
//            return -1;
        }
        break;
    }

//    mVideoCodec = avcodec_find_decoder_by_name("h264_mediacodec");
    qDebug() << "--------------mVideoCodec" << pCodecPar->codec_id << pCodec;


    /* 寻找视频流 */
    int ret = av_find_best_stream(mFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &mVideoCodec, 0); //查找视频流信息;
    if (ret < 0) {
        qDebug() << "Cannot find a video stream in the input file";
    }else{
        mVideoIndex = ret;
        //fps
        int fps_num = mFormatCtx->streams[mVideoIndex]->r_frame_rate.num;
        int fps_den = mFormatCtx->streams[mVideoIndex]->r_frame_rate.den;
        if (fps_den > 0) {
            _fps = fps_num / fps_den;
            if(mCallback)
                mCallback->mediaUpdateFps(_fps);
        }

//        if(mHWConfigList.size() <= 0){ //全例子仅调用一次  调用会导致内存泄露（未找到释放方法）
//            for (int i = 0;; i++) {
//                const AVCodecHWConfig *config = avcodec_get_hw_config(mVideoCodec, i);
//                if (!config) {
//                    break;
//                }
//                if ((config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX) && config->device_type != AV_HWDEVICE_TYPE_NONE) {
//                    mHWConfigList.push_back((AVCodecHWConfig *)config);
//                    qDebug() << "------------------------config->device_type" << config->device_type << config->pix_fmt;
//                    continue;
//                }
//            }
//        }

        if (!(mVideoCodecCtx = avcodec_alloc_context3(mVideoCodec))){
            QThread::msleep(10);
            rePlay();
            return;
        }
        mVideo = mFormatCtx->streams[mVideoIndex];
        if (!mVideoCodecCtx){
            qDebug() << "create video context fail!" << mFormatCtx->filename;
            statusChanged(AVDefine::AVMediaStatus_InvalidMedia);
        }else{
            if (avcodec_parameters_to_context(mVideoCodecCtx, mVideo->codecpar) < 0){
                QThread::msleep(10);
                rePlay();
                return;
            }

//            if(mVideoCodecCtx != NULL){
//                if(avcodec_is_open(mVideoCodecCtx))
//                    avcodec_close(mVideoCodecCtx);
//            }

            mVideoCodecCtxMutex.lockForWrite();
            if(HW && false /*mHWConfigList.size() > 0*/){ //选择硬解
//                for(int i = 0; i < mHWConfigList.size(); i++){
//                    if(mVideoCodecCtx != NULL){
//                        if(avcodec_is_open(mVideoCodecCtx))
//                            avcodec_close(mVideoCodecCtx);
//                    }
//                    const AVCodecHWConfig *config = mHWConfigList.at(mHWConfigList.size() - 1- i);
//                    mHWPixFormat = config->pix_fmt;
//                    mVideoCodecCtx->get_format = get_hw_format; //回调 &mHWDeviceCtx
//                    /** 硬解上下文 */
//                    AVBufferRef *hWDeviceCtx = nullptr;
//                    if ( hw_decoder_init(mVideoCodecCtx, hWDeviceCtx, config) < 0) {
//                        mUseHw = false;
//                    }else{
//                        mUseHw = true;
//                        mVideoCodecCtx->thread_count = 0;
//                        mVideoCodecCtx->opaque = (void *) this;
//                    }
//                    if( mUseHw && avcodec_open2(mVideoCodecCtx, mVideoCodec, NULL) >= 0){
//                        mIsOpenVideoCodec = true;
//                        qDebug() << "-----------------------config->device_type OK" << config->device_type << config->pix_fmt;
//                        break;
//                    }else{
//                        qDebug() << "-----------------------config->device_type NG" << config->device_type << config->pix_fmt << mVideoCodecCtx->hw_device_ctx;
//                        if(mVideoCodecCtx->hw_device_ctx){
//                            av_buffer_unref(&mVideoCodecCtx->hw_device_ctx);
//                            mVideoCodecCtx->hw_device_ctx = nullptr;
//                        }
//                    }
//                }
            }else{
                mVideoCodecCtx->thread_count = 0; //线程数
                //                mVideoCodecCtx->opaque = (void *) this;
                mIsOpenVideoCodec = avcodec_open2(mVideoCodecCtx, mVideoCodec, NULL) >= 0;
                mUseHw = false;
            }
            mVideoCodecCtxMutex.unlock();

            if(mIsOpenVideoCodec){
                AVDictionaryEntry *tag = NULL;
                tag = av_dict_get(mFormatCtx->streams[mVideoIndex]->metadata, "rotate", tag, 0);
//                if(tag != NULL)
//                    vFormat.rotate = QString(tag->value).toInt();
                av_free(tag);
//                vFormat.width = mVideoCodecCtx->width;
//                vFormat.height = mVideoCodecCtx->height;
                //                    vFormat.format = mVideoCodecCtx->pix_fmt;

                if(mCallback){
                    mCallback->mediaHasVideoChanged();
                }
                videoq->setTimeBase(mFormatCtx->streams[mVideoIndex]->time_base);
                videoq->init();

                renderq->init();
                //-- 启动抓包线程 和 解码线程
                getPacketTask();
                decodecTask();
            } else{
                statusChanged(AVDefine::AVMediaStatus_InvalidMedia);
            }

            if(mPixFormat != mVideoCodecCtx->pix_fmt)
                mPixFormat = mVideoCodecCtx->pix_fmt;
            if(mSize != QSize(mVideoCodecCtx->width, mVideoCodecCtx->height) )
                mSize = QSize(mVideoCodecCtx->width, mVideoCodecCtx->height);

            if(!mUseHw) //初始化转换器
            {
                switch (mVideoCodecCtx->pix_fmt) {
                case AV_PIX_FMT_YUV420P :
                case AV_PIX_FMT_NV12 :
                default: //AV_PIX_FMT_YUV420P  如果上面的格式不支持直接渲染，则转换成通用AV_PIX_FMT_YUV420P格式
                    //                        vFormat.format = AV_PIX_FMT_YUV420P;
                    if(mVideoSwsCtx != NULL){
                        sws_freeContext(mVideoSwsCtx);
                        mVideoSwsCtx = NULL;
                    }
                    mVideoSwsCtx = sws_getContext(
                                mVideoCodecCtx->width,
                                mVideoCodecCtx->height,
                                mVideoCodecCtx->pix_fmt,
                                mVideoCodecCtx->width,
                                mVideoCodecCtx->height,
                                (AVPixelFormat)AV_PIX_FMT_YUV420P,// (AVPixelFormat)vFormat.format,
                                SWS_BICUBIC,NULL,NULL,NULL);
                    break;
                }
            }
            initEncodec();

            statusChanged(AVDefine::AVMediaStatus_Buffering);
            mIsInit = true;

            qDebug()<<"jie ma qi de name======    "<<mVideoCodec->name;
            qDebug("init play_video is ok!!!!!!!");
        }
    }
}

static int readRawDataCB(void *opaque, uint8_t *buf, int buf_size)
{
    (void)opaque; (void)buf_size;
    int len = -1;
    while(len < 1)
    {
        len = g_fifo->read(buf);
        QThread::usleep(1);
    }
    return (-1 == len)?AVERROR(errno) :len;
}

void AVDecoder::init2(){
    QMutexLocker locker(&mDeleteMutex);

    g_fifo->clear();
    if(mIsInit)
        return;
    mIsInit = true;
    mIsInit = false;
    statusChanged(AVDefine::AVMediaStatus_Loading);
    //寻找视频
    mFormatCtx = avformat_alloc_context();
//    lastReadPacktTime = av_gettime();
    mFormatCtx->interrupt_callback.opaque = this;

    uint8_t  *avio_ctx_buffer = NULL;
    size_t avio_ctx_buffer_size = 1024000 /*327680*/;// 3072;// 32768;
    AVIOContext *avio_ctx = NULL;
    AVInputFormat *pAVInputFmt = NULL;
    int ret;

    avio_ctx_buffer = (uint8_t *)av_malloc(avio_ctx_buffer_size);
    if (!avio_ctx_buffer) {
        ret = AVERROR(ENOMEM);
    }
    avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
        0, NULL, &readRawDataCB, NULL, NULL);
    if (!avio_ctx) {
        ret = AVERROR(ENOMEM);
    }
    mFormatCtx->pb = avio_ctx;
    mFormatCtx->probesize = 50000000;  //

    ret = av_probe_input_buffer(mFormatCtx->pb, &pAVInputFmt, "", NULL, 0, 0);
    if ( ret< 0)
    {
        qDebug() << QString("Couldn't probe input buffer.\n");
        if (avio_ctx) {
            av_freep(&avio_ctx->buffer);
            av_freep(&avio_ctx);
        }
        QThread::msleep(10);
        rePlay();
        return;
    }

//    AVDictionary* options = NULL;
//    av_dict_set(&options, "buffer_size", "102400", 0);  //增大“buffer_size”
//    av_dict_set(&options, "hwaccel", "h264_mediacodec", 0);  //设置超时断开连接时间
    mFormatCtx->interrupt_callback.opaque = this;
    if (avformat_open_input(&mFormatCtx, "", pAVInputFmt, NULL /*&options*/) != 0) {//打开视频
        qDebug() << QString("Couldn't open input stream.\n");
        if (avio_ctx) {
            av_freep(&avio_ctx->buffer);
            av_freep(&avio_ctx);
        }
        QThread::msleep(10);
        rePlay();
        return;
    }

    if(avformat_find_stream_info(mFormatCtx, NULL) < 0) //判断是否找到视频流信息
    {
        qDebug() << "media find stream error : " << mFilename.toStdString().data();
        statusChanged(AVDefine::AVMediaStatus_InvalidMedia);
        if (avio_ctx) {
            av_freep(&avio_ctx->buffer);
            av_freep(&avio_ctx);
        }
        QThread::msleep(10);
        rePlay();
        return;
    }

/*
    AVCodecParameters *pCodecPar = mFormatCtx->streams[AVMEDIA_TYPE_VIDEO]->codecpar; //查找解码器
    //获取一个合适的编码器pCodec find a decoder for the video stream
    // AVCodec *pCodec = avcodec_find_decoder(pCodecPar->codec_id);
    AVCodec *pCodec;
    switch (pCodecPar->codec_id){
    case AV_CODEC_ID_H264:
        pCodec = avcodec_find_decoder_by_name("h264_mediacodec");//硬解码264
        if (pCodec == NULL) {

            qDebug("Couldn't find Codec.\n");
        }
        break;
    case AV_CODEC_ID_MPEG4:
        pCodec = avcodec_find_decoder_by_name("mpeg4_mediacodec");//硬解码mpeg4
        if (pCodec == NULL) {

            qDebug("Couldn't find Codec.\n");
        } break;
    case AV_CODEC_ID_HEVC:
        pCodec = avcodec_find_decoder_by_name("hevc_mediacodec");//硬解码265
        if (pCodec == NULL) {
            qDebug("Couldn't find Codec.\n");
        }
        break;
    default:
        pCodec = avcodec_find_decoder(pCodecPar->codec_id);//软解
        if (pCodec == NULL) {
            qDebug("Couldn't find Codec.\n");
        }
        break;
    }
//    mVideoCodec = avcodec_find_decoder_by_name("h264_mediacodec");
    qDebug() << "--------------mVideoCodec" << pCodecPar->codec_id << pCodec;
    */

    mVideoCodec =  avcodec_find_decoder(AV_CODEC_ID_H264); //zhi jie fa xian
    if (!mVideoCodec) {
        qDebug() << "Cannot find a video stream in the input file";
    }else{
        mVideoIndex = AVMEDIA_TYPE_VIDEO; //ret;
        //fps
        int fps_num = mFormatCtx->streams[mVideoIndex]->r_frame_rate.num;
        int fps_den = mFormatCtx->streams[mVideoIndex]->r_frame_rate.den;
        if (fps_den > 0) {
            _fps = fps_num / fps_den;
            if(mCallback)
                mCallback->mediaUpdateFps(_fps);
        }

//        //选择硬解
//        mHWConfigList.clear();
//        for (int i = 0;; i++) {
//            const AVCodecHWConfig *config = avcodec_get_hw_config(mVideoCodec, i);
//            if (!config) {
//                break;
//            }
//            if ((config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX) && config->device_type != AV_HWDEVICE_TYPE_NONE) {
//                mHWConfigList.push_back((AVCodecHWConfig *)config);
//                qDebug() << "------------------------config->device_type" << config->device_type << config->pix_fmt;
//                continue;
//            }
//        }

        if (!(mVideoCodecCtx = avcodec_alloc_context3(mVideoCodec))){
            QThread::msleep(10);
            rePlay();
            return;
        }
        mVideo = mFormatCtx->streams[mVideoIndex];
        if (!mVideoCodecCtx){
            qDebug() << "create video context fail!" << mFormatCtx->filename;
            statusChanged(AVDefine::AVMediaStatus_InvalidMedia);
        }else{
            if (avcodec_parameters_to_context(mVideoCodecCtx, mVideo->codecpar) < 0){
                QThread::msleep(10);
                rePlay();
                return;
            }

            mVideoCodecCtxMutex.lockForWrite();
            if(HW && false /*&& mHWConfigList.size() > 0*/){ //选择硬解
//                for(int i = 0; i < mHWConfigList.size(); i++){
//                    if(mVideoCodecCtx != NULL){
//                        if(avcodec_is_open(mVideoCodecCtx))
//                            avcodec_close(mVideoCodecCtx);
//                    }
//                    const AVCodecHWConfig *config = mHWConfigList.at(mHWConfigList.size() - 1- i);
//                    mHWPixFormat = config->pix_fmt;
//                    mVideoCodecCtx->get_format = get_hw_format; //回调 &mHWDeviceCtx
//                    /** 硬解上下文 */
//                    AVBufferRef *hWDeviceCtx = nullptr;
//                    if ( hw_decoder_init(mVideoCodecCtx, hWDeviceCtx, config) < 0) {
//                        mUseHw = false;
//                    }else{
//                        mUseHw = true;
//                        mVideoCodecCtx->thread_count = 0;
//                        mVideoCodecCtx->opaque = (void *) this;
//                    }
//                    if( mUseHw && avcodec_open2(mVideoCodecCtx, mVideoCodec, NULL) >= 0){
//                        mIsOpenVideoCodec = true;
//                        qDebug() << "-----------------------config->device_type OK" << config->device_type << config->pix_fmt;
//                        break;
//                    }else{
//                        qDebug() << "-----------------------config->device_type NG" << config->device_type << config->pix_fmt << mVideoCodecCtx->hw_device_ctx;
//                        if(mVideoCodecCtx->hw_device_ctx){
//                            av_buffer_unref(&mVideoCodecCtx->hw_device_ctx);
//                            mVideoCodecCtx->hw_device_ctx = nullptr;
//                        }
//                    }
//                }
            }else{
                mVideoCodecCtx->thread_count = 0; //线程数
                //                mVideoCodecCtx->opaque = (void *) this;
                mIsOpenVideoCodec = avcodec_open2(mVideoCodecCtx, mVideoCodec, NULL) >= 0;
                mUseHw = false;
            }
            mVideoCodecCtxMutex.unlock();

            if(mIsOpenVideoCodec){
                AVDictionaryEntry *tag = NULL;
                tag = av_dict_get(mFormatCtx->streams[mVideoIndex]->metadata, "rotate", tag, 0);
//                if(tag != NULL)
//                    vFormat.tsSave = QString(tag->value).toInt();
                av_free(tag);
//                vFormat.width = mVideoCodecCtx->width;
//                vFormat.height = mVideoCodecCtx->height;
                //                    vFormat.format = mVideoCodecCtx->pix_fmt;

                if(mCallback){
                    mCallback->mediaHasVideoChanged();
                }
                videoq->setTimeBase(mFormatCtx->streams[mVideoIndex]->time_base);
                videoq->init();
                renderq->init();
            } else{
                statusChanged(AVDefine::AVMediaStatus_InvalidMedia);
            }

            if(mPixFormat != mVideoCodecCtx->pix_fmt)
                mPixFormat = mVideoCodecCtx->pix_fmt;
            if(mSize != QSize(mVideoCodecCtx->width, mVideoCodecCtx->height) )
                mSize = QSize(mVideoCodecCtx->width, mVideoCodecCtx->height);

            if(!mUseHw) //初始化转换器
            {
                switch (mVideoCodecCtx->pix_fmt) {
                case AV_PIX_FMT_YUV420P :
                case AV_PIX_FMT_NV12 :
                default: //AV_PIX_FMT_YUV420P  如果上面的格式不支持直接渲染，则转换成通用AV_PIX_FMT_YUV420P格式
                    //                        vFormat.format = AV_PIX_FMT_YUV420P;
                    if(mVideoSwsCtx != NULL){
                        sws_freeContext(mVideoSwsCtx);
                        mVideoSwsCtx = NULL;
                    }
                    mVideoSwsCtx = sws_getContext(
                                mVideoCodecCtx->width,
                                mVideoCodecCtx->height,
                                mVideoCodecCtx->pix_fmt,
                                mVideoCodecCtx->width,
                                mVideoCodecCtx->height,
                                (AVPixelFormat)AV_PIX_FMT_YUV420P,// (AVPixelFormat)vFormat.format,
                                SWS_BICUBIC,NULL,NULL,NULL);
                    break;
                }
            }
            initEncodec();

            statusChanged(AVDefine::AVMediaStatus_Buffering);
            mIsInit = true;
            //-- 启动抓包线程 和 解码线程
            getPacketTask();
            decodecTask();

            qDebug()<<"jie ma qi de name======    "<<mVideoCodec->name;
            qDebug("init play_video is ok!!!!!!!");
        }
    }
}

void AVDecoder::initEncodec()
{
    if(outUrl.isEmpty())
        return;
    string output = outUrl.toStdString();
    mIsInitEC = OpenOutput(output) == 0;
    emit senderEncodecStatus(mIsInitEC);
}

int AVDecoder::OpenOutput(string outUrl)
{
    inputContext = mFormatCtx;
    int ret  = avformat_alloc_output_context2(&outputContext, nullptr, "flv"/*flv"*/, outUrl.c_str());
    if(ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "open output context failed\n");
        goto Error;
    }

    ret = avio_open2(&outputContext->pb, outUrl.c_str(), AVIO_FLAG_READ_WRITE,nullptr, nullptr);
    if(ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "open avio failed");
        goto Error;
    }
    for(int i = 0; i < inputContext->nb_streams; i++)
    {
        AVStream *in_stream  = inputContext->streams[i];
        if(in_stream->codec->codec_type != AVMEDIA_TYPE_VIDEO) //只解视频数据
            continue;

        AVStream *out_stream = avformat_new_stream(outputContext, in_stream->codec->codec);
        //法一
        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
        //法二
        //            ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
        //            out_stream->codecpar->codec_tag = 0;
        if(ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "copy coddec context failed");
            goto Error;
        }
        out_stream->codec->codec_tag = 0;  //这个值一定要为0
        if (outputContext->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    //av_dump_format(outputContext, 0, outUrl.c_str(), 1);

    //    AVDictionary* options = NULL;
    //    av_dict_set(&options,"rtbufsize","5000",0);
    //    av_dict_set(&options,"start_time_realtime",0,0);
    //    av_opt_set(outputContext->priv_data, "preset", "superfast", 0);
    //    av_opt_set(outputContext->priv_data, "tune", "zerolatency", 0);
    ret = avformat_write_header(outputContext, NULL); //写入参数
    if(ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "format write header failed");
        goto Error;
    }

    av_log(NULL, AV_LOG_FATAL, " Open output file success %s\n",outUrl.c_str());
    return ret ;
Error:
    if(outputContext)
    {
        for(int i = 0; i < outputContext->nb_streams; i++)
        {
            avcodec_close(outputContext->streams[i]->codec);
        }
        avformat_close_input(&outputContext);
    }
    return ret ;
}

void AVDecoder::getPacket()
{
    if(!mIsInit || !mIsOpenVideoCodec){ //必须初始化
        statusChanged(AVDefine::AVMediaStatus_InvalidMedia);
        return;
    }

    AVPacket *pkt = av_packet_alloc();
    mVideoCodecCtxMutex.lockForRead();
//    lastReadPacktTime = av_gettime();
    int ret = av_read_frame(mFormatCtx, pkt);
    mVideoCodecCtxMutex.unlock();
    if ((ret == AVERROR_EOF || avio_feof(mFormatCtx->pb))) { //读取完成
        av_packet_unref(pkt);
        av_freep(pkt);
        getPacketTask(1);
        return;
    }

    static uchar errorSum = 0;
    if (mFormatCtx->pb && mFormatCtx->pb->error)
    {
        av_packet_unref(pkt);
        av_freep(pkt);
        if(errorSum++ > 20){ //一直报错 重启
            rePlay();
        }
        getPacketTask(1);
        return;
    }else{
        errorSum = 0;
    }

    if(ret < 0)
    {
        av_packet_unref(pkt);
        av_freep(pkt);
        getPacketTask(1);
        return;
    }

    //        qDebug() << "-----------------------pkt->stream_index" << pkt->stream_index <<   mVideoIndex;
    if (pkt->stream_index == mVideoIndex && mIsOpenVideoCodec){
        //计算真实的渲染FPS
        _fpsFrameSum++;

        if(mIsInitEC){  //转RTMP流
            AVPacket *videopacket_t = av_packet_alloc();
            av_copy_packet(videopacket_t, pkt);
            if (av_dup_packet(videopacket_t) < 0)
            {
                av_free_packet(videopacket_t);
            }
            videopacket_t->pos = 0;
            videopacket_t->flags = 1;
            videopacket_t->convergence_duration = 0;
            videopacket_t->side_data_elems = 0;
            videopacket_t->stream_index = 0;
            videopacket_t->duration = 0;

            auto inputStream = inputContext->streams[videopacket_t->stream_index];
            auto outputStream = outputContext->streams[0];  //只有视屏
            av_packet_rescale_ts(videopacket_t,inputStream->time_base,outputStream->time_base);

            int ret = av_interleaved_write_frame(outputContext, videopacket_t);  //推流
            av_freep(videopacket_t);
            //            qDebug() << "-----------------------------stream_index" << ret;
        }

        if(tsSave) //保存TS
            fwrite(pkt->data, 1, pkt->size, tsSave);//写数据到文件中

        pkt->pts = pkt->pts == AV_NOPTS_VALUE ? pkt->dts : pkt->pts;
        videoq->put(pkt);   //显示
//        qDebug() << << "-----------------------------stream_index" << pkt;
    }else {
        av_packet_unref(pkt);
        av_freep(pkt);
    }
    getPacketTask(1);
}

void AVDecoder::decodec()
{
    if(!mIsInit || !mIsOpenVideoCodec){ //必须初始化
        statusChanged(AVDefine::AVMediaStatus_InvalidMedia);
        return;
    }

    if(videoq->size() > 20) //清空
        videoq->release();

    if(videoq->size() <= 0) {
        decodecTask(1);
        return;
    }

    AVPacket *pkt = videoq->get();
    if (pkt->stream_index == mVideoIndex) {
        int ret = decode_write(mVideoCodecCtx, pkt);
//                qDebug() << "------------------------------decodec" << ret <<videoq->size() ;
    }
    av_packet_unref(pkt);
    av_freep(pkt);
    decodecTask(1);
}

int AVDecoder::decode_write(AVCodecContext *avctx, AVPacket *packet)
{
    AVFrame *frame = NULL, *sw_frame = NULL;

    int ret = 0;
    mVideoCodecCtxMutex.lockForRead();

    ret = avcodec_send_packet(avctx, packet);  // ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,packet);
    if (ret < 0) {
        qWarning() << "Error during decoding";
        mVideoCodecCtxMutex.unlock();
        return ret;
    }

    AVFrame *tmp_frame = av_frame_alloc();
    sw_frame = av_frame_alloc();
    frame = sw_frame;

    while (avcodec_receive_frame(avctx, frame) == 0) {
        ret = 0;
        //        ret = avcodec_receive_frame(avctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            av_frame_unref(sw_frame);
            av_frame_free(&sw_frame);
            mVideoCodecCtxMutex.unlock();
            return 0;
        } else if (ret < 0) {
            qWarning() << "Error while decoding";
            goto fail;
        }

        if(mSize != QSize(frame->width, frame->height)){
            /// 重要信息变更 需要转码时重启
            rePlay();
        }

        if (mUseHw) { // 硬解拷贝
            /* retrieve data from GPU to CPU */
            if(frame->format == mHWPixFormat ){
                if ((ret = av_hwframe_transfer_data(tmp_frame, frame, 0)) < 0) {
                    qWarning() << "Error transferring the data to system memory";

                    goto fail;
                }else
                    av_frame_copy_props(tmp_frame, frame);
            }
        } else{ //软解 转格式  --- [软解还没调好]  >> BUG 函数结束时 item->frame被清空 导致无数据
            if(mPixFormat != frame->format){
                /// 重要信息变更 需要转码时重启
                rePlay();
            }

            if(mVideoSwsCtx && frame->format != AV_PIX_FMT_YUV420P){
                ret = sws_scale(mVideoSwsCtx,
                                frame->data,
                                frame->linesize,
                                0,
                                frame->height,
                                tmp_frame->data,
                                tmp_frame->linesize);

                if(ret < 0){
                    qWarning() << "Error sws_scale";
                    goto fail;
                }
            }else{  //内存移动 -- 不然会被释放
                av_frame_move_ref(tmp_frame, frame);
            }
        }
        if(tmp_frame->pts != AV_NOPTS_VALUE){
            tmp_frame->pts = av_q2d(mFormatCtx->streams[mVideoIndex]->time_base ) * tmp_frame->pts * 1000;
        }
        if(renderq->size() > 20){
            //goto fail;
//            renderq->release();
            renderq->put(tmp_frame);
        }else{
           renderq->put(tmp_frame);
        }
        av_frame_unref(sw_frame);
        av_frame_free(&sw_frame);
        mVideoCodecCtxMutex.unlock();
        return ret;
    }

fail:
    av_frame_unref(tmp_frame);
    av_frame_free(&tmp_frame);
    av_frame_unref(sw_frame);
    av_frame_free(&sw_frame);
    mVideoCodecCtxMutex.unlock();
    return ret;
}

void AVDecoder::setFilenameImpl(const QString &source){

    qDebug() << "------------------AVDecoder::setFilenameImpl" << source;
    if(mFilename == source) return;
    if(mFilename.size() > 0){
        release();//释放所有资源
    }
    mFilename = source;
    mIsOpenVideoCodec = false;
    mIsInit = false;
    if(mFilename.size() > 0)
        load();
}

void AVDecoder::_rePlay()
{
    if(mFilename.size() > 0){
        release();//释放所有资源
    }
    mIsOpenVideoCodec = false;
    mIsInit = false;
    if(mFilename.size() > 0){
        QThread::msleep(1);
        load();
    }
}

void AVDecoder::requestRender()
{
    /* 1-60  >2 45  >3 30  >4 20  >5 15 */
    int len = renderq->size();
    if(len >= 1){
        static qint64 lastTime2 = QDateTime::currentMSecsSinceEpoch();
        int space3 =QDateTime::currentMSecsSinceEpoch() - lastTime2; //no data time

        qint64 lastTime = QDateTime::currentMSecsSinceEpoch();

        qint64 currentTime = requestRenderNextFrame(); //x显示


        int space = QDateTime::currentMSecsSinceEpoch() - lastTime;

//        qDebug() << "----------------------- len" << len << _fps << space;
        int frameStep = 1000 / (_fps +3)/* + 1*/;
//        int space2  = frameStep;
//        if(len > 5){
//            space = frameStep * 2 / 3;
//        }else if(len > 1){
//            space = space2 - space - 2;
//        }else{
//            space = space2 - space + 1;
//        }
//        space -= space3;
//        if(space <= 0){
//            space = 20;
//        }else if(space > frameStep){
//            space = frameStep;
//        }
        space = frameStep - space;
//        qDebug() << "----------------------- len2  << space" << len  <<frameStep << space << space3;

//        QThread::msleep(space);

//        QThread::msleep(frameStep);
        lastTime2 = QDateTime::currentMSecsSinceEpoch();
    }
    wakeupPlayer();
}



/* ------------------------------------------------------私有函数-------------------------------------------------------------- */

void AVDecoder::statusChanged(AVDefine::AVMediaStatus status){
    if(mStatus == status)
        return;
    mStatus = status;
    if(mCallback)
        mCallback->mediaStatusChanged(status);
}

void AVDecoder::release(bool isDeleted){
    qDebug() << "_----------------------------RES0";
    mDecodeThread.clearAllTask(); //清除所有任务
    mProcessThread.clearAllTask(); //清除所有任务
    if(videoq){
        videoq->release();
    }
    if(renderq){
        renderq->release();
    }

    qDebug() << "_----------------------------RES1";
    mVideoCodecCtxMutex.lockForWrite();
    if(mVideoCodecCtx != NULL){
        if(avcodec_is_open(mVideoCodecCtx))
            avcodec_close(mVideoCodecCtx);
//        if(mVideoCodecCtx->hw_device_ctx){
//            av_buffer_unref(&mVideoCodecCtx->hw_device_ctx);
//            mVideoCodecCtx->hw_device_ctx = nullptr;
//        }
        avcodec_free_context(&mVideoCodecCtx);
        mVideoCodecCtx = NULL;
    }
    mVideoCodecCtxMutex.unlock();

#if defined(Q_OS_ANDROID)
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
        if(mVideoCodec != NULL){
            av_free(mVideoCodec);
            mVideoCodec = NULL;
        }
#endif
        qDebug() << "_----------------------------RES3";
    if(mFormatCtx != NULL){
        if( mIsOpenVideoCodec ){
            av_read_pause(mFormatCtx);
            avformat_close_input(&mFormatCtx);
        }
        avformat_free_context(mFormatCtx);
        mFormatCtx = NULL;
    }
    if(mVideoSwsCtx != NULL){
        sws_freeContext(mVideoSwsCtx);
        mVideoSwsCtx = NULL;
    }
    if(mRGBSwsCtx != NULL){
        sws_freeContext(mRGBSwsCtx);
        mRGBSwsCtx = NULL;
    }
    qDebug() << "_----------------------------RES4";
    if(outputContext != NULL){
        for(int i = 0; i < outputContext->nb_streams; i++)
        {
            avcodec_close(outputContext->streams[i]->codec);
        }
        avformat_close_input(&outputContext);
        avformat_free_context(outputContext);
        outputContext = NULL;
    }
    if(_out_buffer != NULL){
        av_free(_out_buffer);
        _out_buffer = NULL;
    }
    if(_frameRGB != NULL){
        av_frame_unref(_frameRGB);
        av_free(_frameRGB);
        _frameRGB = NULL;
    }
    if(tsSave){
        fclose(tsSave);
        tsSave = nullptr;
    }
    statusChanged(AVDefine::AVMediaStatus_UnknownStatus);
    if(mCallback)
        mCallback->formatCleanUpCallback();
}

/* ------------------------显示函数---------------------------------- */

qint64 AVDecoder::requestRenderNextFrame(){
    qint64 time = 0;

    if(renderq->size()){
        AVFrame* frame = renderq->get();
        VlcVideoFrame* _frame = nullptr;
        if(mCallback /* 0*/){
            mCallback->lockCallback((void**) &_frame);
            if(!_frame->inited){
                _frame->inited = true;
                _frame->width  = frame->width;
                _frame->height = frame->height;
                _frame->planeCount = 3;

                for (unsigned int i = 0; i < _frame->planeCount; ++i) {
                    if(i == 0){
                        _frame->pitch[i] = _frame->width;
                        _frame->lines[i] = _frame->height      ;
                        _frame->visiblePitch[i] = _frame->pitch[i];
                        _frame->visibleLines[i] = _frame->lines[i];
                    }else{
                        _frame->pitch[i] = _frame->width >> 1;
                        _frame->lines[i] =  _frame->height >> 1;
                        _frame->visiblePitch[i] = _frame->pitch[i];
                        _frame->visibleLines[i] = _frame->lines[i];
                    }
                    _frame->plane[i].resize(_frame->pitch[i] * _frame->lines[i]);
                }
            }

            uint8_t* y = (uint8_t*)_frame->plane[0].data();
            for(int i = 0; i < _frame->height; i++){ //将y分量拷贝
                uint8_t* srcy = frame->data[0] + frame->linesize[0] * i;
                ::memcpy(y, srcy, _frame->width);
                srcy += frame->linesize[0] ;
                y += _frame->width ;
            }

            int uvH = _frame->height >> 1;
            uint8_t* u = (uint8_t*)_frame->plane[1].data();
            uint8_t* v = (uint8_t*)_frame->plane[2].data();
            if((AVPixelFormat)frame->format == AV_PIX_FMT_NV12){
//                qDebug() << "-------------------->>HW" << frame->linesize[1] << uvH  << _frame->width;
                for(int i = 0; i < uvH; i++){ //将uv分量拷贝
                    uint8_t* uv = frame->data[1] + frame->linesize[1] * i;
                    for(int j = 0; j < (_frame->width >> 1); j++){
                        *v++ = *uv++;
                        *u++ = *uv++;
                    }
                }
            }else if((AVPixelFormat)frame->format == AV_PIX_FMT_YUV420P){
//                qDebug() << "-------------------->>" << frame->linesize[1] << frame->linesize[2] << frame->linesize[3] << frame->linesize[4] << uvH  << _frame->width;
                for(int i = 0; i < uvH; i++){ //将uv分量拷贝
                    uint8_t* srcV = frame->data[1] + frame->linesize[1] * i;
                    uint8_t* srcU = frame->data[2] + frame->linesize[2] * i;  // + (_frame->width >> 1) + 16;
                    ::memcpy(v, srcV, (_frame->width >> 1));
                    ::memcpy(u, srcU, (_frame->width >> 1));
                    srcV += frame->linesize[1] ;
                    srcU += frame->linesize[2] ;
                    v += (_frame->width >> 1) ;
                    u += (_frame->width >> 1) ;
                }
            }
             mCallback->unlockCallback();
        }
        if(_isSaveImage){ //保存图片
            saveFrame(frame);
            }

        av_frame_unref(frame);
        av_free(frame);
    }
    return time;
}

/* ------------------------------------------------------队列操作-------------------------------------------------------------- */
void AVDecoder::onFpsTimeout()
{
    return;
    if(!mIsInit || !mIsOpenVideoCodec){ //必须初始化
        return;
    }

    if(renderq->size()<= 0) {
        return;
    }

    if(20 < _fpsFrameSum && _fpsFrameSum < 62){
        _fps = (_fpsFrameSum + _fps) / 2;
        if(mCallback)
            mCallback->mediaUpdateFps(_fps);
    }
    _fpsFrameSum = 0;
}
void AVDecoder::saveFrame(AVFrame *frame)
{
    _isSaveImage = false;
    if(!frame)
        return;
    if(mRGBSwsCtx == NULL){
        mRGBSwsCtx = sws_getContext(
                    frame->width,
                    frame->height,
                    (AVPixelFormat)frame->format,
                    frame->width,
                    frame->height,
                    (AVPixelFormat)AV_PIX_FMT_RGB24,// (AVPixelFormat)vFormat.format,
                    SWS_BICUBIC,NULL,NULL,NULL);
    }

    if(mRGBSwsCtx != nullptr){
        if(!_frameRGB){
            _frameRGB = av_frame_alloc();
            qint64  numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, frame->width, frame->height);
            _out_buffer  = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));//内存分配
            avpicture_fill((AVPicture *) _frameRGB, _out_buffer, AV_PIX_FMT_RGB24,
                           frame->width, frame->height);
        }

        int ret = sws_scale(mRGBSwsCtx,
                            frame->data,
                            frame->linesize,
                            0,
                            frame->height,
                            _frameRGB->data,
                            _frameRGB->linesize);

        if(ret < 0){
            qWarning() << "Error sws_scale";
        }else{
            QString dir = QString("%1").arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + "/MMC Station/Media/Photo/" + QDateTime::currentDateTime().toString("yyyyMMdd") + "/";
            QDir tmpDir1;
            bool ok = false;
            if(tmpDir1.exists(dir)){
                ok = true;
            }else{
                ok = tmpDir1.mkpath(dir);
            }
            if(ok){
                QString fileName = QDateTime::currentDateTime().toString("hhmmsszzz");
                QImage img =  QImage(_frameRGB->data[0], frame->width, frame->height, QImage::Format_RGB888);
                img.save(QString("%1%2%3").arg(dir).arg(fileName).arg(".jpg"));
            }
        }
    }
}

