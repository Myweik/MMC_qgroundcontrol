﻿#ifndef TRANSCEIVERMANAGER_H
#define TRANSCEIVERMANAGER_H

#pragma once

#include "radiomember.h"
#include "radioprovider.h"
#include "androidraduimember.h"
#include "../fpv/fpvmember.h"

#include <QGCToolbox.h>
#include <QString>
#include <QObject>

/**
 ** class TransceiverManager
 * 用来管理电台及图传配置
 */

#include <QObject>

class TransceiverManager : public QGCTool
{
    Q_OBJECT
public:
    explicit TransceiverManager(QGCApplication *app, QGCToolbox *toolbox);
    ~TransceiverManager();

    Q_PROPERTY(RadioMemberBase*             radioMember                READ radioMember               NOTIFY radioMemberChanged)

    /* 数传部分 */
    void disconnectRadio  (void);
    void connectRadio (const QString& device);

    RadioMemberBase* radioMember() { return _radioMember; }

    // Override from QGCTool
    virtual void setToolbox(QGCToolbox *toolbox);
signals:
    void radioMemberChanged();

private slots:
    void onStartTimer();
    void RTCMDataUpdate(int type, QByteArray msg);

private:
    void setRadioMember(RadioMemberBase* radioMember);
    void cleanup();

    RadioProvider*   _radioProvider = nullptr;
    RadioMemberBase* _radioMember   = nullptr;

    FpvMember*      _fpvMember      = nullptr;

//    std::atomic_bool _requestRadioStop; ///< signals the thread to quit
    bool _isconnect = false;
    QTimer*    _serialLostTimer;

    int _maxCount = 0;
};


#endif // TRANSCEIVERMANAGER_H
