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

#ifndef VLCQT_QMLVIDEOPLAYER_H_
#define VLCQT_QMLVIDEOPLAYER_H_

#include <QtQuick/QQuickItem>

#include "QmlVideoObject.h"

/*!
    \class VlcQmlVideoPlayer QmlVideoPlayer.h VLCQtQml/QmlVideoPlayer.h
    \ingroup VLCQtQml
    \brief QML video player (deprecated)

    A simple QML video player that can be used standalone and directly inside QML.

    \deprecated Deprecated since VLC-Qt 1.1, will be removed in 2.0
 */
class Q_DECL_DEPRECATED  VlcQmlVideoPlayer : public VlcQmlVideoObject
{
Q_OBJECT
public:
    /*!
        \brief Current aspect ratio
        \see setAspectRatio
        \see aspectRatioChanged
     */
    Q_PROPERTY(QString aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)

    /*!
        \brief Current crop ratio
        \see setCropRatio
        \see cropRatioChanged
     */
    Q_PROPERTY(QString cropRatio READ cropRatio WRITE setCropRatio NOTIFY cropRatioChanged)

    /*!
        \brief Current media URL
        \see setUrl
     */
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)

    /*!
        \brief Current media record
        \see setRecord
     */
    Q_PROPERTY(bool record READ record WRITE setRecord NOTIFY recordChanged)


    /*!
        \brief VlcQmlVideoPlayer constructor.
        \param parent parent item (QQuickItem *)
     */
    explicit VlcQmlVideoPlayer(QQuickItem *parent = 0);

    /*!
        VlcMediaPlayer destructor
     */
    ~VlcQmlVideoPlayer();

    /*!
        \brief Register QML plugin as VLCQt.VlcVideoPlayer

        Include into QML file as
        import VLCQt VERSION_MAJOR.VERSION_MINOR

        Object name: VlcVideoPlayer
     */
    static void registerPlugin();

    /*!
        \brief Pause current playback

        Invokable from QML.
     */
	Q_INVOKABLE void pause();

    /*!
        \brief Play current playback

        Invokable from QML.
     */
	Q_INVOKABLE void play();

    /*!
        \brief Stop current playback

        Invokable from QML.
     */
	Q_INVOKABLE void stop();

    /*!
        \brief save Image

        Invokable from QML.
     */
    Q_INVOKABLE void saveImage();

    /*!
        \brief Get current aspect ratio
        \return current aspect ratio

        Used as property in QML.
     */
    QString aspectRatio() const;
    void setAspectRatio(const QString &aspectRatio);

    /*!
        \brief Get current crop ratio
        \return current crop ratio

        Used as property in QML.
     */
    QString cropRatio() const;
    void setCropRatio(const QString &cropRatio);

    /*!
        \brief Get current media URL
        \return current media URL

        Used as property in QML.
     */
    QUrl url() const;
    void setUrl(const QUrl &url);

    /*!
        \brief Get current media URL
        \return current media URL

        Used as property in QML.
     */
    bool record() const;
    void setRecord(const bool state);


signals:
    void urlChanged();
    void recordChanged();
    void aspectRatioChanged();
    void cropRatioChanged();

private slots:

private:
    QUrl _url;
    bool _record = false;
};

#endif // VLCQT_QMLVIDEOPLAYER_H_
