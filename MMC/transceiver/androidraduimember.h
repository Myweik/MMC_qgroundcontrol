#ifndef ANDROIDRADUIMEMBER_H
#define ANDROIDRADUIMEMBER_H

#include <QObject>
#include "radiomemberbase.h"

class AndroidRaduiMember : public RadioMemberBase
{
    Q_OBJECT
public:
    explicit AndroidRaduiMember(QObject *parent = nullptr);

    void analysisPack(int type, QByteArray msg);
    QString radioSettingUrl(){ return "AndroidRadioSettingUrl.qml"; }
signals:

public slots:
};

#endif // ANDROIDRADUIMEMBER_H
