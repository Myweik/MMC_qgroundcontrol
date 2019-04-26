#include "RtspPlayer.h"
#include <QDebug>
//#include "QGCApplication.h"

RtspPlayer::RtspPlayer(QObject *parent)
    : QObject(parent)
    , m_format(QSize(1920, 1080), QVideoFrame::Format_NV12)
    , m_surface(nullptr)
    , m_decoder(nullptr)
{

}

RtspPlayer::~RtspPlayer()
{

}

void RtspPlayer::setVideoSurface(QAbstractVideoSurface *surface)
{
    //qDebug() << surface->supportedPixelFormats();

    if (m_surface == surface)
        return;

    if (m_surface && m_surface->isActive()) {
        m_surface->stop();
    }

    m_surface = surface;

    if (m_surface)
        m_surface->start(m_format);
}

void RtspPlayer::setDecoder(AVDecoder *decoder)
{
    if (m_decoder == decoder)
        return;

    if (m_decoder) {
        disconnect(m_decoder, SIGNAL(frameSizeChanged(int,int)),
                   this, SLOT(onFrameSizeChanged(int,int)));

        disconnect(m_decoder, SIGNAL(newVideoFrame(QVideoFrame)),
                   this, SLOT(onNewVideoFrameReceived(QVideoFrame)));

        //m_decoder->stop();
    }

    m_decoder = decoder;

    if (m_decoder) {
        connect(m_decoder, SIGNAL(frameSizeChanged(int,int)),
                this, SLOT(onFrameSizeChanged(int,int)));

        connect(m_decoder, SIGNAL(newVideoFrame(QVideoFrame)),
                this, SLOT(onNewVideoFrameReceived(QVideoFrame)));
    }

    emit decoderChanged(m_decoder);
}

void RtspPlayer::onFrameSizeChanged(int width, int height)
{
    m_format.setFrameSize(width, height);

    emit widthChanged(width);
    emit heightChanged(height);

    if (m_surface)
        m_surface->start(m_format);
}

void RtspPlayer::onNewVideoFrameReceived(const QVideoFrame &frame)
{
//    static qint64 lastTime = QDateTime::currentMSecsSinceEpoch();

//    if(QDateTime::currentMSecsSinceEpoch() -lastTime > 70 || QDateTime::currentMSecsSinceEpoch() -lastTime < 10)
//        qDebug() << "---------------------onNewVideoFrameReceived" << QDateTime::currentMSecsSinceEpoch() - lastTime;

    if (m_surface){
        m_surface->present(frame);
    }
//    lastTime = QDateTime::currentMSecsSinceEpoch();
}

void RtspPlayer::setWidth(int width)
{
    if (this->width() == width)
        return;

    m_format.setFrameSize(width, this->height());

    emit widthChanged(width);
}

void RtspPlayer::setHeight(int height)
{
    if (this->height() == height)
        return;

    m_format.setFrameSize(this->width(), height);

    emit heightChanged(height);
}

void RtspPlayer::setSize(int width, int height)
{
    m_format.setFrameSize(width, height);

    emit widthChanged(width);
    emit heightChanged(height);
}
