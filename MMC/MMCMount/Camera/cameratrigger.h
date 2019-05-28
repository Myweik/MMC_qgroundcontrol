#ifndef CAMERATRIGGER_H
#define CAMERATRIGGER_H

#include <QObject>
#include <QGeoCoordinate>

class CameraTrigger : public QObject
{
    Q_OBJECT
public:
    explicit CameraTrigger(QObject *parent = 0);
    CameraTrigger(double latitude, double longitude, QObject *parent = 0);

    Q_PROPERTY(QGeoCoordinate coordinate        READ coordinate  WRITE setCoordinate NOTIFY coordinateChanged)

    QGeoCoordinate  coordinate(void){ return _coordinate; }

    void setCoordinate(QGeoCoordinate coordinate);
signals:
    void coordinateChanged (QGeoCoordinate coordinate);
public slots:

private:
    QGeoCoordinate _coordinate;
};

#endif // CAMERATRIGGER_H
