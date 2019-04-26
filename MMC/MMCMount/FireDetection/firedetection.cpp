#include "firedetection.h"


void FireDetection::calcu_picture_GPS(double latitude, double longitude, double altitude, char latitude_Ref, char longitude_Ref,
                                      double equivalent_focal_length, double pitch_angle, double yaw_angle, double roll_angle, float proportion)
{
    //计算等效焦距
    calcu_picture_angle(equivalent_focal_length);
    //数据存储和角度转化为弧度
    roll_angle = roll_angle / 180 * M_PI;
    pitch_angle = pitch_angle / 180.0 * M_PI;
    yaw_angle = yaw_angle / 180 * M_PI;
    pitch_angle += (90.0 / 180) * M_PI;
    if(altitude < 0)
    {
        altitude =0.0001;
    }
    latitudeRec = latitude;
    longitudeRec = longitude;
    altitudeRec = altitude;
    latRef = latitude_Ref;
    lonRef = longitude_Ref;
    pitchRec = pitch_angle;
    yawRec = yaw_angle;
    rollRec = roll_angle;
    proportionRec = proportion;
    //画面有角度超度水平线
    if(pitch_angle > (0.5 * M_PI - picture_angle_ / 2 ) && pitch_angle < 0.5 * M_PI)
    {
        outOfRangeAngle = pitch_angle - (0.5 * M_PI - picture_angle_ / 2 ) ;
        outOfRangeAngle = (1 - tan(0.5 * M_PI - pitch_angle) / tan(picture_angle_ / 2)) / 2;
        emit outOfRangeAngleChanged(outOfRangeAngle);
        calcu_vertical_pos(picture_angle_,altitudeRec,proportionRec,outOfRangeAngle);
        calcu_roll(roll_angle);
        calcu_pitch(pitch_angle);
        intersection(altitudeRec);
        calcu_yaw(yaw_angle);
        calcuCenterPosition();
//        calcu_GPS(latitudeRec,longitudeRec,latitude_Ref,longitude_Ref);
        return;
    }
    else if(pitch_angle <= 0)
    {
        pitch_angle = 0.01 / 180 * M_PI;
    }
    //画面角度在90度内
    outOfRangeAngle = -1;
    emit outOfRangeAngleChanged(outOfRangeAngle);
//    calcu_picture_angle(equivalent_focal_length);
    calcu_vertical_pos(picture_angle_,altitudeRec,proportionRec);
    calcu_roll(roll_angle);
    calcu_pitch(pitch_angle);
    intersection(altitudeRec);
    calcu_yaw(yaw_angle);
    calcu_GPS(latitudeRec,longitudeRec,latitude_Ref,longitude_Ref);
    calcuCenterPosition();
}

//等效焦距求视角
void FireDetection::calcu_picture_angle(double picture_angle_num)
{
    double  l;
    //35mm等效焦距的图片尺寸
    _senor_width = 36;
    _senor_height = 24;

    l = sqrt(pow(_senor_width,2) + pow(_senor_height,2));
    picture_angle_ = 2 * qAtan(l / (2*picture_angle_num));

}

//4个GPS坐标
void FireDetection::calcu_vertical_pos(double picture_angle, double altitude,float proportion)
{
    double l,x,y;
    //计算垂直向下拍摄的对角线长
    l = tan(picture_angle / 2) * altitude * 2;
    //偏移的X、Y
    x = l / 2 * (1 / sqrt(1 + pow(proportion,2)));
    y = l / 2 * (proportion / sqrt(1 + pow(proportion,2)));
    for(int i = 0; i < 4; i++)
    {
        vertical_pos[i].x = (i-1 <= 0) ? x : -x;
        vertical_pos[i].y = (i & 0x01) > 0 ? -y : y;
        vertical_pos[i].z = -altitude;
    }

    GPScenter.x = 0;
    GPScenter.y = 0;
    GPScenter.z = -altitude;
}

void FireDetection::calcu_vertical_pos(double picture_angle, double altitude, float proportion, double out_of_range_angle)
{
    double l,x,y;
    //计算垂直向下拍摄的对角线长
    l = tan(picture_angle / 2) * altitude * 2;
    x = l / 2 * (1 / sqrt(1 + pow(proportion,2)));
    y = l / 2 * (proportion / sqrt(1 + pow(proportion,2)));
    for(int i = 0; i < 4; i++)
    {
//        vertical_pos[i].x = (i-1 <= 0) ? x : -x;
        vertical_pos[i].y = (i & 0x01) > 0 ? -y : y;
        vertical_pos[i].z = -altitude;
    }
//    vertical_pos[0].y = y * tan(picture_angle / 2 - out_of_range_angle) / tan(picture_angle / 2);
//    vertical_pos[1].y = -y;
//    vertical_pos[2].y = y * tan(picture_angle / 2 - out_of_range_angle) / tan(picture_angle / 2);
//    vertical_pos[3].y = -y;
    vertical_pos[0].x = x * tan(picture_angle / 2 - out_of_range_angle) / tan(picture_angle / 2);
    vertical_pos[1].x = x * tan(picture_angle / 2 - out_of_range_angle) / tan(picture_angle / 2);
    vertical_pos[2].x = -x;
    vertical_pos[3].x = -x;
    GPScenter.x = 0;
    GPScenter.y = 0;
    GPScenter.z = -altitude;
}
//先滚转
void FireDetection::calcu_roll(double roll_degree)
{
    struct coordin temp;
    for(int i = 0;i < 4;i++)
    {
        temp.x = vertical_pos[i].x;
        temp.y = vertical_pos[i].y;
        temp.z = vertical_pos[i].z;
        vertical_pos[i].x = temp.x;
        vertical_pos[i].y = temp.y * cos(roll_degree) + temp.z * sin(roll_degree);
        vertical_pos[i].z = temp.z * cos(roll_degree) - temp.y * sin(roll_degree);
    }
    temp.x = GPScenter.x;
    temp.y = GPScenter.y;
    temp.z = GPScenter.z;
    GPScenter.x = temp.x;
    GPScenter.y = temp.y * cos(roll_degree) + temp.z * sin(roll_degree);
    GPScenter.z = temp.z * cos(roll_degree) - temp.y * sin(roll_degree);
}
//再俯仰
void FireDetection::calcu_pitch(double pitch_degree)
{
    struct coordin temp;
    for(int i = 0; i < 4;i++)
    {
        temp.x = vertical_pos[i].x;
        temp.y = vertical_pos[i].y;
        temp.z = vertical_pos[i].z;
        vertical_pos[i].y = temp.y;
        vertical_pos[i].z = temp.z * cos(pitch_degree) + temp.x * sin(pitch_degree);
        vertical_pos[i].x = temp.x * cos(pitch_degree) - temp.z * sin(pitch_degree);
    }
    temp.x = GPScenter.x;
    temp.y = GPScenter.y;
    temp.z = GPScenter.z;
    GPScenter.y = temp.y;
    GPScenter.z = temp.z * cos(pitch_degree) + temp.x * sin(pitch_degree);
    GPScenter.x = temp.x * cos(pitch_degree) - temp.z * sin(pitch_degree);
}

//求出与地平面四个交点
void FireDetection::intersection(double altitude)
{
    for(int i = 0;i < 4;i++)
    {
        ground_pos[i].x = vertical_pos[i].x / vertical_pos[i].z * (-altitude);
        ground_pos[i].y = vertical_pos[i].y / vertical_pos[i].z * (-altitude);
        ground_pos[i].z = -altitude;
        no_yaw_pos[i].x = ground_pos[i].x;
        no_yaw_pos[i].y = ground_pos[i].y;
    }
    center_pos.x = GPScenter.x / GPScenter.z * (-altitude);
    center_pos.y = GPScenter.y / GPScenter.z * (-altitude);
    center_pos.z = -altitude;
}

//根据偏航调整坐标
void FireDetection::calcu_yaw(double yaw_degree)
{
    struct coordin temp;
    for(int i = 0;i < 4;i++)
    {
        temp.x = ground_pos[i].x;
        temp.y = ground_pos[i].y;
        ground_pos[i].x = temp.x * cos(yaw_degree) + temp.y * sin(yaw_degree);
        ground_pos[i].y = temp.y * cos(yaw_degree) - temp.x * sin(yaw_degree);
    }
    temp.x = center_pos.x;
    temp.y = center_pos.y;
    center_pos.x = temp.x * cos(yaw_degree) + temp.y * sin(yaw_degree);
    center_pos.y = temp.y * cos(yaw_degree) - temp.x * sin(yaw_degree);
}

void FireDetection::calcu_yaw(double yaw_degree, struct coordin* pos)
{
    struct coordin temp;
    temp.x = pos->x;
    temp.y = pos->y;
    pos->x = temp.x * cos(yaw_degree) + temp.y * sin(yaw_degree);
    pos->y = temp.y * cos(yaw_degree) - temp.x * sin(yaw_degree);
}

//计算现有GPS
void FireDetection::calcu_GPS(double latitude, double longitude, char latitudeRef, char longitudeRef)
{
    switch(judge_hemisphere(latitudeRef,longitudeRef))
    {
        case 0:                         //  N E
            calcu_pos(latitude,longitude,'N','E');
            break;
        case 1:                         //  N W
            calcu_pos(latitude,longitude,'N','W');
            break;
        case 2:                         //  S E
            calcu_pos(latitude,longitude,'S','E');
            break;
        case 3:                         //  S W
            calcu_pos(latitude,longitude,'S','W');
            break;
        default:
            qDebug()<<"ref is error";
            break;
    }
}

//判断半球位置
int FireDetection::judge_hemisphere(char latitudeRef, char longitudeRef)
{
    switch(latitudeRef)
    {
        case 'N':
            switch(longitudeRef)
            {
                case 'E':
                    return 0;
                break;
                case 'W':
                    return 1;
                break;
                default:
                break;
            }

            break;
        case 'S':
            switch(longitudeRef)
            {
                case 'E':
                    return 2;
                break;
                case 'W':
                    return 3;
                break;
                default:
                break;
            }
            break;
        default:
            break;
    }
    return -1;
}

//计算    根据当地GPS坐标
void FireDetection::calcu_pos(double latitude, double longitude, char latitudeRef, char longitudeRef)
{
    latitudeRef = latitudeRef;
    longitudeRef = longitudeRef;
    double  r_lat;
    for(int i = 0;i < 4;i++)
    {
        GPS[i].setLatitude(latitude + ground_pos[i].y / (R_EQUATOR * 2 * M_PI / 360));
        r_lat = R_EQUATOR * cos((GPS[i].latitude() / 180) * M_PI);
        GPS[i].setLongitude(longitude + ground_pos[i].x / (r_lat * 2 * M_PI / 360));
//        if(GPS[i].longitude() > 180)
//        {
//            qDebug()<<judge_hemisphere(latitudeRef,longitudeRef);
//        }
//        double  tmp = GPS[i].latitude();
//                tmp = GPS[i].longitude();
    }

}

void FireDetection::calcu_pos(double latitude, double longitude, char latitudeRef, char longitudeRef, QGeoCoordinate* gpsPoint,struct coordin* pos)
{
    latitudeRef = latitudeRef;
    longitudeRef = longitudeRef;
    double  r_lat;
    gpsPoint->setLatitude(latitude + (center_pos.x + pos->y - shifting.y) / (R_EQUATOR * 2 * M_PI / 360));
    r_lat = R_EQUATOR * cos((gpsPoint->latitude() / 180) * M_PI);
    gpsPoint->setLongitude(longitude + (- center_pos.y + pos->x - shifting.x) / (r_lat * 2 * M_PI / 360));
}

void FireDetection::calcuArea()
{
    sum = 0;
    linelength = 0;
    if(GPS_store.count() < 2)
        return;
    QList<QGeoCoordinate> tmplist;
    QGeoCoordinate tmp;

    for(int i = 0;i < GPS_store.count();i++)
    {
        tmp.setLatitude( GPS_store[i].value<QGeoCoordinate>().latitude() / 360 * 2 * M_PI * R_EQUATOR);
        double r_lat = R_EQUATOR * cos(GPS_store[i].value<QGeoCoordinate>().latitude() / 180 * M_PI);
        tmp.setLongitude(GPS_store[i].value<QGeoCoordinate>().longitude() / 360 * 2 * M_PI * r_lat);
        tmplist.append(tmp);
    }
    // line length
    linesectionlength.clear();
    for(int i = 1;i < tmplist.count();i++)
    {
        if(pointmode)
        linesectionlength << QVariant::fromValue(GPS_store[i].value<QGeoCoordinate>().distanceTo(GPS_store[i - 1].value<QGeoCoordinate>()));
        linelength +=GPS_store[i].value<QGeoCoordinate>().distanceTo(GPS_store[i - 1].value<QGeoCoordinate>());
    }
    if(GPS_store.count() >= 3)
    {
        linelength +=GPS_store[0].value<QGeoCoordinate>().distanceTo(GPS_store[GPS_store.length() - 1].value<QGeoCoordinate>());
        if(pointmode)
            linesectionlength << GPS_store[0].value<QGeoCoordinate>().distanceTo(GPS_store[GPS_store.length() - 1].value<QGeoCoordinate>());
    }
    linelength = linelength ;
    if(GPS_store.count() < 3)
        return;
    // area
    for(int i = 1;i<tmplist.count() - 1; i++)
    {
        sum +=(tmplist[i].latitude() - tmplist[0].latitude()) * (tmplist[i + 1].longitude() - tmplist[0].longitude()) - (tmplist[i].longitude() - tmplist[0].longitude()) * (tmplist[i + 1].latitude() - tmplist[0].latitude());
    }
    sum = abs(sum / 2.0 * 1000000);
}

void FireDetection::calcu_square_point(coordin square_point, coordin* square_calcu, double altitude, double pitch_angle,double proportion)
{
    proportion = proportion;
    double hypotenuse,right_angle1_side,right_angle2_side,pic_height,temp_angle1,temp_angle2;
    double pic_h_short;
    double pic_width_angle = 0;
    //照片到中心点距离
    right_angle1_side = altitude / cos(pitch_angle);

    //照片梯形矮的那一半的高度
    pic_width_angle = 2 * atan(tan(picture_angle_ / 2) * 9 / sqrt(pow(9,2) + pow(16,2)));
    pic_h_short = sin(pic_width_angle / 2) / sin(0.5 * M_PI + pitch_angle - pic_width_angle / 2) * right_angle1_side;
    ladder_h_short = pic_h_short;
    //照片垂直投影的高度
    pic_height = 2 * right_angle1_side * tan(pic_width_angle / 2);
    //计算相对于中心位置的高度
    if(square_point.y - 450 > 0)
    {
        right_angle2_side = (square_point.y - 450 ) / 900.0 * pic_height;
        temp_angle1 = atan(right_angle2_side / right_angle1_side);
        temp_angle2 = 0.5 * M_PI - pitch_angle - temp_angle1;
        hypotenuse = sin(temp_angle1) / sin(temp_angle2) * right_angle1_side;
        square_calcu->y = hypotenuse;
    }
    else
    {
        right_angle2_side = (450 - square_point.y) / 900.0 * pic_height;
        temp_angle1 = atan(right_angle2_side / right_angle1_side);
        temp_angle2 = 0.5 * M_PI - pitch_angle;
        double temp_angle3,temp_angle4;
        temp_angle3 = temp_angle1 + temp_angle2;
        temp_angle4 = 0.5 * M_PI - temp_angle1;
        hypotenuse = sin(temp_angle4) / sin(temp_angle3) * right_angle2_side;
        square_calcu->y = - hypotenuse;
    }
}

void FireDetection::clearGpsStore()
{
    if(stopped == true)
    {
        square_store.clear();
        linesectionlength.clear();
        GPS_store.clear();
        deal_num = 0;
        emit gpsStoreChanged(GPS_store);
    }

}

void FireDetection::calcu_points(double altitude,double pitch,double yaw,double proportion, int x, int y)
{
    //相对于底边的高度   相对于中线的位置
    square_point_.x = x;
    square_point_.y = y;

    square_point_.y = 900 - square_point_.y;
    square_point_.x = square_point_.x - 900 * proportion / 2;

    ladder_top = no_yaw_pos[2].y - no_yaw_pos[3].y;
    ladder_bottom = no_yaw_pos[0].y - no_yaw_pos[1].y;

    ladder_height = no_yaw_pos[0].x - no_yaw_pos[2].x;
    ladder_h1 = ladder_top / ( ladder_bottom - ladder_top) * ladder_height;
    calcu_square_point(square_point_,&square_calcu_,altitude,pitch,proportion);
    square_calcu_.x = (square_calcu_.y + ladder_h1 + ladder_h_short) / ladder_h1 * ladder_top * square_point_.x / (900 * proportion);
    calcu_yaw(yaw,&square_calcu_);
    calcu_pos(latitudeRec,longitudeRec,latRef,lonRef,&GPS_Point,&square_calcu_);
//    QGeoCoordinate *cameratrigger = new QGeoCoordinate;
//    cameratrigger->setCoordinate(GPS_Point);
//    double tmp = GPS_Point.latitude();
//    tmp = GPS_Point.longitude();
    GPS_store<<QVariant::fromValue(GPS_Point);

}

void FireDetection::calcuPoint(double altitude, double pitch, double yaw, double proportion, int x, int y)
{
    //相对于底边的高度   相对于中线的位置
    square_point_.x = x;
    square_point_.y = y;

    square_point_.y = 900 - square_point_.y;
    square_point_.x = square_point_.x - 900 * proportion / 2;

    ladder_top = no_yaw_pos[2].y - no_yaw_pos[3].y;
    ladder_bottom = no_yaw_pos[0].y - no_yaw_pos[1].y;

    ladder_height = no_yaw_pos[0].x - no_yaw_pos[2].x;
    ladder_h1 = ladder_top / ( ladder_bottom - ladder_top) * ladder_height;
    calcu_square_point(square_point_,&square_calcu_,altitude,pitch,proportion);
    square_calcu_.x = (square_calcu_.y + ladder_h1 + ladder_h_short) / ladder_h1 * ladder_top * square_point_.x / (900 * proportion);
    calcu_yaw(yaw,&square_calcu_);
    calcu_pos(latitudeRec,longitudeRec,latRef,lonRef,&GPS_Point,&square_calcu_);
    qfirepoint_ = QVariant::fromValue(GPS_Point);
    qfirepointChanged(qfirepoint_);
}

void FireDetection::stopThread(bool stop)
{
    stopped = stop;
}

void FireDetection::square(QList<QPoint> trans_point)
{
    square_store.append(trans_point);
}

void FireDetection::square(QPoint tmp)
{
    firepointrec.setX(tmp.x());
    firepointrec.setY(tmp.y());
}

void FireDetection::setdealnum(int num)
{
    deal_num = num;
}


void FireDetection::run()
{
    while(!stopped)
    {
        startflag = false;
        if(firepoint)
        {
            calcuPoint(altitudeRec,pitchRec,yawRec,proportionRec,firepointrec.x(),firepointrec.y());
            stopped = true;
            calcufinish = true;
            emit calcufinishChanged(calcufinish);
        }
        else if(deal_num < square_store.count())
        {
            calcu_points(altitudeRec,pitchRec,yawRec,proportionRec,square_store[deal_num].x(),square_store[deal_num].y());
            deal_num++;
        }
        else
        {
            square_store.clear();
            emit gpsStoreChanged(GPS_store);
            deal_num = 0;
            calcuArea();

            if(startflag == false)
            {
                stopped = true;
                calcufinish = true;
                emit calcufinishChanged(calcufinish);
            }
        }
    }
}

void FireDetection::calcuCenterPosition()
{
    square_point_.x = 800;
    square_point_.y = 450;

    square_point_.y = 900 - square_point_.y;
    square_point_.x = square_point_.x - 900 * proportionRec / 2;

    ladder_top = no_yaw_pos[2].y - no_yaw_pos[3].y;
    ladder_bottom = no_yaw_pos[0].y - no_yaw_pos[1].y;

    ladder_height = no_yaw_pos[0].x - no_yaw_pos[2].x;
    ladder_h1 = ladder_top / ( ladder_bottom - ladder_top) * ladder_height;
    calcu_square_point(square_point_,&shifting,altitudeRec,pitchRec,proportionRec);
    shifting.x = (shifting.y + ladder_h1 + ladder_h_short) / ladder_h1 * ladder_top * square_point_.x / (900 * proportionRec);
    calcu_yaw(yawRec,&shifting);
}

FireRec::FireRec(QObject *parent) : QObject(parent)
{
    firepoint_ = false;
    firedetection = new FireDetection;
    firedetection->stopThread(true);
    firedetection->setOutOfRangeAngle(-1);
    firedetection->setfirepoint(false);
    firedetection->setdealnum(0);
    firedetection->setcalcufinish(true);
    calcufinish_ = true;
    calcufinishChanged(calcufinish_);
    drawfinish = false;
    connect(firedetection,SIGNAL(gpsStoreChanged(QVariantList)),this,SLOT(recgpsstore(QVariantList)));
    connect(firedetection,SIGNAL(calcufinishChanged(bool)),this,SLOT(reccalcufinish(bool)));
    connect(firedetection,SIGNAL(outOfRangeAngleChanged(double)),this,SLOT(recOutOfRangeAngle(double)));
    connect(firedetection,SIGNAL(qfirepointChanged(QVariant)),this,SLOT(qfirepointrec(QVariant)));
}

void FireRec::square(int x, int y, int num)
{
    QPoint point_tmp;
    point_tmp.setX(x);
    point_tmp.setY(y);
    square_store.append(point_tmp);
    if(num == square_store.length())
    {
        if(!firedetection->readstopped())
        {
            square_store.clear();
            return;
        }
        firedetection->square(square_store);

        firedetection->setStart(true);
        firedetection->setcalcufinish(false);
        firedetection->stopThread(false);
        firedetection->start();

        square_store.clear();
    }


}

void FireRec::square(int x, int y)
{
    if(!firedetection->readstopped())
    {
        return;
    }
    QPoint tmp;
    tmp.setX(x);
    tmp.setY(y);
    firedetection->square(tmp);
    firedetection->setStart(true);
    firedetection->setcalcufinish(false);
    firedetection->stopThread(false);
    firedetection->start();
}

void FireRec::calcu_picture_GPS(double latitude, double longitude, double altitude, char latitude_Ref, char longitude_Ref, double equivalent_focal_length, double pitch_angle, double yaw_angle, double roll_angle, float proportion)
{
    latitude_Ref = latitude_Ref;
    longitude_Ref = longitude_Ref;
    roll_angle = roll_angle;
    firedetection->calcu_picture_GPS(latitude,longitude,(altitude),'N','E',equivalent_focal_length,pitch_angle,yaw_angle,0,proportion);
}

void FireRec::clearGpsStore()
{
    if(firedetection->readstopped() == true)
    {
        firedetection->clearGpsStore();
        GPS_store.clear();
        emit gpsStoreChanged(GPS_store);
    }
}



void FireRec::recgpsstore(QVariantList recstore)
{
    GPS_store.clear();
    GPS_store = recstore;
    emit gpsStoreChanged(GPS_store);
    _fireLines.clear();
    if(!pointmode_ || GPS_store.count() < 2)
        return;
    for(int i = 1; i < GPS_store.count(); i++)
    {
        CoordinateVector *vect = new CoordinateVector(GPS_store[i-1].value<QGeoCoordinate>(), GPS_store[i].value<QGeoCoordinate>(), false);
        _fireLines.append(vect);
    }
    if(GPS_store.count() > 2)
    {
        CoordinateVector *vect = new CoordinateVector(GPS_store[GPS_store.count()-1].value<QGeoCoordinate>(), GPS_store[0].value<QGeoCoordinate>(), false);
        _fireLines.append(vect);
    }
    emit fireLinesChanged(&_fireLines);

}

void FireRec::reccalcufinish(bool tmp)
{
    calcufinish_ = tmp;
    emit calcufinishChanged(calcufinish_);
}


// n e ok
void FireRec::calcuArea()
{
    sum = firedetection->readSum();
    emit sumAreaChanged(sum);
}
void FireRec::calculine()
{
    linelength = firedetection->readLineLength();
    emit lineLengthChanged(linelength);
    linesectionlength_.clear();
    if(pointmode_)
    {
        linesectionlength_ = firedetection->readlinesectionlength();
        emit linesectionlengthChanged(linesectionlength_);
    }

}

void FireRec::setPointmode(bool tmp)
{
    pointmode_ = tmp;
    pointmodeChanged(pointmode_);
    firedetection->setPointMode(pointmode_);
}

void FireRec::setFirepoint(bool tmp)
{
    firepoint_ = tmp;
    firedetection->setfirepoint(tmp);
    emit firepointChanged(firepoint_);
}

QString FireRec::getFilePath()
{
    QString dir = QString("%1").arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + "/MMC Station/";
    return dir;
}

QString FireRec::isDirExist()
{
    QFileInfo fi("D:/");
    if(fi.exists())
        return "file:///" + fi.filePath();
    return "file:///C:/Users/Administrator/Desktop";
}

void FireRec::qfirepointrec(QVariant tmp)
{
    qfirepoint_.clear();
    qfirepoint_.append(tmp);
    emit qfirepointChanged(qfirepoint_);
}
void FireRec::recOutOfRangeAngle(double tmp)
{
    outOfRangeAngle_ = tmp;
    emit outOfRangeAngleChanged(outOfRangeAngle_);
}
