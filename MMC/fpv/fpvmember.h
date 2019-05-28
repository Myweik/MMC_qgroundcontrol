#ifndef FPVMEMBER_H
#define FPVMEMBER_H

#include <QObject>
#include "fpvdatamutual.h"

class FpvMember : public QObject
{
    Q_OBJECT
public:
    explicit FpvMember(QObject *parent = nullptr);

signals:

public slots:

private:
    fpvDataMutual* _fpvSerial;
};

#endif // FPVMEMBER_H
