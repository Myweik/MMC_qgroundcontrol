#ifndef FPVMEMBER_H
#define FPVMEMBER_H

#include <QObject>
//#include "fpvdatamutual.h"
#include "../qtavplayer/src/usbexample.h"
#include "QGC.h"

class FpvMember : public QObject
{
    Q_OBJECT
    MMC_GENERATE_QML_PROPERTY(pairStatus, bool, "pairStatus")
    MMC_GENERATE_QML_PROPERTY(VT, uint, "VT") //0x11
    MMC_GENERATE_QML_PROPERTY(RC, uint, "RC")

public:
    explicit FpvMember(QObject *parent = nullptr);

signals:

public slots:
    void onReadyReadData(QByteArray array);
    Q_INVOKABLE void setConfig();

private:
//    fpvDataMutual* _fpvSerial;
    UsbExample*      _hidUSB = nullptr;
};

#endif // FPVMEMBER_H
