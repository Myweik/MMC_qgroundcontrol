/****************************************************************************
* VLC-Qt - Qt and libvlc connector library
* Copyright (C) 2013 Tadej Novak <tadej@tano.si>
*
* Based on Phonon multimedia library
* Copyright (C) 2011 Harald Sitter <sitter@kde.org>
*
* This library is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

//#include "VLCQtCore/MediaPlayer.h"

#include "QmlVideoObject.h"


#include <QTimer>
#include <QByteArray>
#include <QBuffer>

VlcQmlVideoObject::VlcQmlVideoObject(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      _geometry(0, 0, 640, 480),
      _boundingRect(0, 0, 0, 0),
      _frameSize(0, 0),
      _graphicsPainter(0),
      _paintedOnce(false),
      _gotSize(false),
      _aspectRatio(Vlc::Original),
      _cropRatio(Vlc::Original),
      _decoder(new AVDecoder)
{
    setRenderTarget(InvertedYFramebufferObject);
    setFlag(ItemHasContents, true);

    _decoder->setMediaCallback(this);

}

VlcQmlVideoObject::~VlcQmlVideoObject()
{
    if (_graphicsPainter)
        delete _graphicsPainter;
    if(_decoder)
        delete _decoder;

}

QRectF VlcQmlVideoObject::boundingRect() const
{
    return _boundingRect;
}

void VlcQmlVideoObject::updateBoundingRect()
{
    _boundingRect = QRectF(0, 0, _frameSize.width(), _frameSize.height());

    updateAspectRatio();

    QSizeF scaledFrameSize = _boundingRect.size();
    if (_aspectRatio == Vlc::Ignore) {
        scaledFrameSize.scale(_geometry.size(), Qt::IgnoreAspectRatio);
    } else {
        scaledFrameSize.scale(_geometry.size(), Qt::KeepAspectRatio);
    }
    _boundingRect.setSize( scaledFrameSize );

    updateCropRatio();

    _boundingRect.moveCenter(_geometry.center());
}

void VlcQmlVideoObject::updateAspectRatio()
{
    QSizeF ar = Vlc::ratioSize( _aspectRatio );

    if( ar.width() != 0 && ar.height() != 0)
    {
        qreal ratio = qMin( _boundingRect.width() / ar.width() , _boundingRect.height() / ar.height() );
        _boundingRect.setWidth( (qreal) ratio * ar.width() );
        _boundingRect.setHeight( (qreal) ratio * ar.height() );
    }
}

void VlcQmlVideoObject::updateCropRatio()
{
    QSizeF ar = Vlc::ratioSize( _cropRatio );

    if( ar.width() != 0 && ar.height() != 0)
    {
        QRectF cropRect = _boundingRect;
        qreal ratio = qMin( cropRect.width() / ar.width() , cropRect.height() / ar.height() );
        cropRect.setWidth( (qreal) ratio * ar.width() );
        cropRect.setHeight( (qreal) ratio * ar.height() );

        QSizeF scaledFrameSize = cropRect.size();
        scaledFrameSize.scale(_geometry.size(), Qt::KeepAspectRatio);


        _boundingRect.setWidth( _boundingRect.width() * ( scaledFrameSize.width() / cropRect.width() ) );
        _boundingRect.setHeight( _boundingRect.height() * ( scaledFrameSize.height() / cropRect.height() ) );
    }
}

Vlc::Ratio VlcQmlVideoObject::cropRatio() const
{
    return _cropRatio;
}

void VlcQmlVideoObject::setCropRatio(const Vlc::Ratio &cropRatio)
{
    _cropRatio = cropRatio;
    updateBoundingRect();
}

Vlc::Ratio VlcQmlVideoObject::aspectRatio() const
{
    return _aspectRatio;
}

void VlcQmlVideoObject::setAspectRatio(const Vlc::Ratio &aspectRatio)
{
    _aspectRatio = aspectRatio;
    updateBoundingRect();
}

void VlcQmlVideoObject::paint(QPainter *painter)
{
    lock();
    if( _frame.inited )
    {
        if (!_graphicsPainter)
            _graphicsPainter = new GlslPainter;

        Q_ASSERT(_graphicsPainter);

        _gotSize = false;
        if (!_gotSize || _frameSize.isNull()) {
            // TODO: do scaling ourselfs?
            _gotSize = true;
            _frameSize = QSize(_frame.width, _frame.height);
            updateBoundingRect();
        }

        if (!_paintedOnce) {
            painter->fillRect(_boundingRect, Qt::black);
            _paintedOnce = true;
        } else {
            Q_ASSERT(_graphicsPainter);
            _graphicsPainter->setFrame(&_frame);
            if (!_graphicsPainter->inited())
                _graphicsPainter->init();
            _graphicsPainter->paint(painter, _boundingRect, this); //显示
        }
    }else{
        qDebug() << "----------------------------------------unlock";
    }
//    _frame.inited = false;
    unlock();
}

void VlcQmlVideoObject::geometryChanged(const QRectF &newGeometry,
                                          const QRectF &oldGeometry)
{
    _geometry = newGeometry;
    updateBoundingRect();

    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

void VlcQmlVideoObject::frameReady()
{
    update();
}

void VlcQmlVideoObject::reset()
{
    // Do not reset the spyFormats as they will not change.
    _paintedOnce = false;
    _gotSize = false;

    // The painter is reset because the backend may choose another format for
    // another file (better conversion for some codec etc.)
    if (_graphicsPainter) {
        delete _graphicsPainter;
        _graphicsPainter = 0;
    }
}

//void VlcQmlVideoObject::connectToMediaPlayer(VlcMediaPlayer *player)
//{
//    setCallbacks(player);
//}

//void VlcQmlVideoObject::disconnectFromMediaPlayer(VlcMediaPlayer *player)
//{
//    // Try to prevent callbacks called after this object is being deleted
//    if (player) {
//        player->stop();
//    }

//    unsetCallbacks(player);
//}

void VlcQmlVideoObject::lock()
{
    _mutex.lock();
}

bool VlcQmlVideoObject::tryLock()
{
    return _mutex.tryLock();
}

void VlcQmlVideoObject::unlock()
{
    _mutex.unlock();
}

void *VlcQmlVideoObject::lockCallback(void **planes)
{
    lock();

    *planes = &_frame;

//    for (unsigned int i = 0; i < _frame.planeCount; ++i) {
//        planes[i] = reinterpret_cast<void *>(_frame.plane[i].data());
//    }

    return 0; // There is only one buffer, so no need to identify it.
}

void VlcQmlVideoObject::unlockCallback()
{
    unlock();

    // To avoid thread polution do not call frameReady directly, but via the
    // event loop.
    QMetaObject::invokeMethod(this, "frameReady", Qt::QueuedConnection);
}

void VlcQmlVideoObject::formatCleanUpCallback()
{
    _frame.inited = false;
    // To avoid thread polution do not call reset directly but via the event loop.
    QMetaObject::invokeMethod(this, "reset", Qt::QueuedConnection);
}
