/****************************************************************************
* VLC-Qt - Qt and libvlc connector library
* Copyright (C) 2013 Tadej Novak <tadej@tano.si>
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

#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>

#include "QmlVideoPlayer.h"
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>

VlcQmlVideoPlayer::VlcQmlVideoPlayer(QQuickItem *parent)
    : VlcQmlVideoObject(parent)
    , _url(QUrl())
{


}

VlcQmlVideoPlayer::~VlcQmlVideoPlayer()
{

}

void VlcQmlVideoPlayer::registerPlugin()
{
    qmlRegisterType<VlcQmlVideoPlayer>("VLCQt", 1, 1, "VlcVideoPlayer");
}


QUrl VlcQmlVideoPlayer::url() const
{
    return _url;
}

void VlcQmlVideoPlayer::setUrl(const QUrl &url)
{
    if(url == _url) return;
    _url = url;
    emit urlChanged();

    if(_decoder)
        _decoder->setFilename(url.toString());
}

bool VlcQmlVideoPlayer::record() const
{
    return _record;
}

void VlcQmlVideoPlayer::setRecord(const bool state)
{
    if(_record == state)
        return;
    _record = state;
    emit recordChanged();
    if(_decoder)
        _decoder->saveTs(_record);
}

void VlcQmlVideoPlayer::pause()
{

}

void VlcQmlVideoPlayer::play()
{

}

void VlcQmlVideoPlayer::stop()
{

}

void VlcQmlVideoPlayer::saveImage()
{
    if(_decoder)
        _decoder->saveImage();
}

QString VlcQmlVideoPlayer::aspectRatio() const
{
    return Vlc::ratio()[VlcQmlVideoObject::aspectRatio()];
}

void VlcQmlVideoPlayer::setAspectRatio(const QString &aspectRatio)
{
    VlcQmlVideoObject::setAspectRatio( (Vlc::Ratio) Vlc::ratio().indexOf(aspectRatio) );
    emit aspectRatioChanged();
}

QString VlcQmlVideoPlayer::cropRatio() const
{
    return Vlc::ratio()[VlcQmlVideoObject::cropRatio()];
}

void VlcQmlVideoPlayer::setCropRatio(const QString &cropRatio)
{
    VlcQmlVideoObject::setCropRatio( (Vlc::Ratio) Vlc::ratio().indexOf(cropRatio) );
    emit cropRatioChanged();
}
