#include "cameratrigger.h"

CameraTrigger::CameraTrigger(QObject *parent) : QObject(parent)
{

}

CameraTrigger::CameraTrigger(double latitude, double longitude, QObject *parent) : QObject(parent)
{
    _coordinate.setLatitude(latitude);
    _coordinate.setLongitude(longitude);
    emit coordinateChanged(_coordinate);
}

void CameraTrigger::setCoordinate(QGeoCoordinate coordinate)
{
    _coordinate = coordinate;
    emit coordinateChanged(_coordinate);
}

