#ifndef FIREDETECTION_H
#define FIREDETECTION_H

#include <QObject>
#include <QtMath>
#include <QGeoCoordinate>
#include <QThread>
#include "QmlObjectListModel.h"
#include "CoordinateVector.h"
#include "MMCMount/Camera/cameratrigger.h"
#include <QStandardPaths>
#include <QFileInfo>
#define R_EQUATOR   6371.393

class FireDetection : public QThread
{
    Q_OBJECT
public:
    struct coordin
    {
        double x;
        double y;
        double z;
    };
    void transSquarePoint(double x,double y){ square_point_.x = x; square_point_.y = y; }
/*  Name: 计算画面四角GPS坐标
 *  Param:  纬度 精度 高度 纬度参考方向 精度参考方向
 *          相机等效焦距 俯仰角 偏航角 滚转角 画幅默认比例
 *  return: None
 */
    void    calcu_picture_GPS(double latitude, double longitude, double altitude, char latitude_Ref, char longitude_Ref,
                                  double equivalent_focal_length, double pitch_angle, double yaw_angle, double roll_angle,float proportion);
/*  Name:   计算选点面积各点坐标
 *  Param:  高度 俯仰角 偏航角 画幅默认比例 点在画面中的位置
 *  return: None
 */
    void    calcu_points(double altitude,double pitch,double yaw,double proportion, int x, int y);
/*  Name:   计算选点火点坐标
 *  Param:  高度 俯仰角 偏航角 画幅默认比例 点在画面中的位置
 *  return: None
 */
    void    calcuPoint(double altitude,double pitch,double yaw,double proportion, int x, int y);
    void    stopThread(bool stop);              //停止线程
    void    square(QList<QPoint> trans_point);  //接收用户画的点集合
    void    square(QPoint tmp);                 //接收用户画的单个点
    void    clearGpsStore(void);                //清除缓存

    double  readSum(void){return sum;}                          //读取计算面积
    double  readLineLength(void){return linelength;}            //读取计算的总线长
    bool    readstopped(void){return stopped;}                  //读取线程是否停止
    bool    readcalcufinish(void){return calcufinish;}          //读取计算是否完成
    double  readOutOfRangeAngle(void){return outOfRangeAngle;}  //读取画面中超过水平面的角度
    bool    readPointMode(void){return pointmode;}              //读取是否是画点模式
    void    setdealnum(int num);                                //设置处理点的总数
    void    setStart(bool tmp){startflag = tmp;}                //设置开始标志位
    void    setcalcufinish(bool tmp){calcufinish = tmp; emit calcufinishChanged(calcufinish);}                      //设置计算结束标志
    void    setOutOfRangeAngle(double tmp){outOfRangeAngle = tmp; emit outOfRangeAngleChanged(outOfRangeAngle);}    //设置画面中超过水平面的角度
    void    setPointMode(bool tmp){pointmode = tmp;}            //设置画点模式
    void    setfirepoint(bool tmp){firepoint = tmp;}            //设置是否是火点模式
    QVariantList    readlinesectionlength(void){return linesectionlength;}  //读取分段线长
    QVariant        readqfirepoint(void){return qfirepoint_;}               //读取火点坐标

protected:
    void    run();
private:
    void        calcuCenterPosition();                                      //求出当前画面相对于飞机坐标的偏移
    void        calcu_picture_angle(double picture_angle_num);              //等效焦距求视角
    void        calcu_vertical_pos(double picture_angle ,double altitude,float proportion);                             //竖直向下4个GPS坐标
    void        calcu_vertical_pos(double picture_angle ,double altitude,float proportion,double out_of_range_angle);   //竖直向下4个GPS坐标(角度有超出水平面时)
    void        calcu_roll(double roll_degree);                             //先滚转
    void        calcu_pitch(double pitch_degree);                           //再俯仰
    void        intersection(double altitude);                              //求出与地平面四个交点
    void        calcu_yaw(double yaw_degree);                               //根据偏航调整坐标
    void        calcu_yaw(double yaw_degree,struct coordin* pos);           //根据偏航调整坐标
    void        calcu_GPS(double latitude,double longitude,char latitudeRef,char longitudeRef); //计算现有GPS（暂时没有用上）
    int         judge_hemisphere(char latitudeRef,char longitudeRef);                           //判断半球位置（暂时没有用上）
    void        calcu_pos(double latitude,double longitude,char latitudeRef,char longitudeRef);                                             //计算坐标    根据当地GPS坐标
    void        calcu_pos(double latitude,double longitude,char latitudeRef,char longitudeRef,QGeoCoordinate* gpsPoint,struct coordin* pos);//计算坐标    根据当地GPS坐标
    void        calcuArea(void);                                            //计算面积和线长
    //计算传进来点的坐标
    void        calcu_square_point(struct coordin square_point,struct coordin* square_calcu,double altitude,double pitch_angle,double proportion);


signals:
    void        gpsStoreChanged(QVariantList gpsStore);
    void        calcufinishChanged(bool tmp);
    void        outOfRangeAngleChanged(double tmp);
    void        qfirepointChanged(QVariant tmp);
private:

    struct                      coordin square_point_,square_calcu_,shifting;     //输入点坐标     计算点坐标
    double                      picture_angle_,latitudeRec,longitudeRec,altitudeRec,pitchRec,rollRec,yawRec,proportionRec;
    char                        latRef,lonRef;
    struct                      coordin vertical_pos[4],GPScenter,center_pos,ground_pos[4],no_yaw_pos[4];   //垂直点四个坐标  姿态计算完四个坐标
    QGeoCoordinate              GPS[4],GPS_Point;               //最终的GPS坐标
    double                      ladder_top,ladder_bottom,ladder_height,ladder_h1,ladder_h_short,sum_area;    //照片点对应实际的位置
    QList<QPoint>               square_store;                   //处理点的集合
    int                         deal_num;                       //点的总数量
    volatile bool               stopped;                        //停止标志位
    QVariantList                GPS_store;                      //计算完的GPS坐标
    QVariantList                linesectionlength;              //分段的线长
    bool                        startflag;                      //开始标志位
    double                      sum,linelength;                 //面积    总线长
    bool                        calcufinish,pointmode;          //计算完成和画点模式的标志位
    double                      outOfRangeAngle;                //画面中超过水平面的角度
    bool                        firepoint;                      //火点模式
    QPoint                      firepointrec;                   //接收火点在画幅中的位置
    QVariant                    qfirepoint_;                    //火点经纬度坐标

    double                      _senor_width,_senor_height;   //相机传感器宽度和高度
};


class FireRec : public QObject
{
    Q_OBJECT
public:
    explicit FireRec(QObject *parent = 0);
    Q_PROPERTY(QVariantList     gpsStore            READ    gpsStore            NOTIFY  gpsStoreChanged)            //存储计算的GPS坐标
    Q_PROPERTY(QVariantList     linesectionlength   READ    linesectionlength   NOTIFY  linesectionlengthChanged)   //存储分段线长
    Q_PROPERTY(QVariantList     qfirepoint          READ    qfirepoint          NOTIFY qfirepointChanged)           //火点坐标
    Q_PROPERTY(double   sumArea         READ sumArea            NOTIFY sumAreaChanged)
    Q_PROPERTY(bool     calcufinish     READ calcufinish        NOTIFY calcufinishChanged)                          //计算的面积
    Q_PROPERTY(double   lineLength      READ lineLength         NOTIFY lineLengthChanged)                           //分段线长
    Q_PROPERTY(double   outOfRangeAngle READ outOfRangeAngle    NOTIFY outOfRangeAngleChanged)                      //画面中超过水平面的角度
    Q_PROPERTY(bool     drawFinish      READ drawFinish     WRITE setDrawFinish NOTIFY drawFinishChanged)           //绘制完成标志
    Q_PROPERTY(bool     firepoint       READ firepoint      WRITE setFirepoint  NOTIFY firepointChanged)            //火点模式标志
    Q_PROPERTY(bool     pointmode       READ pointmode      WRITE setPointmode  NOTIFY pointmodeChanged)            //画点模式标志
    Q_PROPERTY(QmlObjectListModel*  fireLines               READ fireLines      NOTIFY fireLinesChanged)            //火线（传入地图）
    QmlObjectListModel* fireLines() { return &_fireLines;}                                                          //读取火线（传入地图）
    Q_INVOKABLE void    square(int x,int y,int num);                                                                //接收用户画的点集合
    Q_INVOKABLE void    square(int x,int y);                                                                        //接收用户画的单个点
    /*  Name: 计算画面四角GPS坐标
     *  Param:  纬度 精度 高度 纬度参考方向 精度参考方向
     *          相机等效焦距 俯仰角 偏航角 滚转角 画幅默认比例
     *  return: None
     */
    Q_INVOKABLE void    calcu_picture_GPS(double latitude, double longitude, double altitude, char latitude_Ref, char longitude_Ref,
                                double equivalent_focal_length, double pitch_angle, double yaw_angle, double roll_angle,float proportion);
    Q_INVOKABLE void    clearGpsStore(void);            //清除缓存GPS
    Q_INVOKABLE void    calcuArea( void );              //计算面积
    Q_INVOKABLE void    calculine(void);                //计算总线长
    Q_INVOKABLE QString     getFilePath(void);          //获取存储读取
    Q_INVOKABLE QString     isDirExist(void);           //判断D盘是否存在，若存在，默认打开D盘
    QVariantList    gpsStore(void){return GPS_store;}   //读取存储GPS
    QVariantList    linesectionlength(void){return linesectionlength_;} //读取分段线长
    QVariantList    qfirepoint(void){return qfirepoint_;}               //读取火点坐标
    bool        drawFinish(void)        {return drawfinish;}            //读取是否绘制完成
    bool        calcufinish(void)       {return calcufinish_;}          //读取是否计算完成
    bool        firepoint(void)         {return firepoint_;}            //读取火点坐标
    bool        pointmode(void)         {return pointmode_;}            //读取画点模式
    double      lineLength(void)        {return linelength;}            //读取总线长
    double      sumArea(void)           {return sum;}                   //读取面积
    double      outOfRangeAngle(void)   {return outOfRangeAngle_;}      //读取画面中超过水平面的角度
    void        setPointmode(bool tmp);                                 //设置画点模式
    void        setFirepoint(bool tmp);                                 //设置火点模式
    void        setDrawFinish(bool tmp){drawfinish = tmp; emit drawFinishChanged(drawfinish);}//设置绘制完成
signals:
    void    gpsStoreChanged(QVariantList GPS_store);
    void    linesectionlengthChanged(QVariantList tmp);
    void    drawFinishChanged(bool drawfinish_);
    void    sumAreaChanged(double sum_);
    void    calcufinishChanged(bool calcufinish_);
    void    lineLengthChanged(double    tmp);
    void    firepointChanged(bool   tmp);
    void    fireLinesChanged(QmlObjectListModel* list);
    void    outOfRangeAngleChanged(double tmp);
    void    pointmodeChanged(bool tmp);
    void    qfirepointChanged(QVariantList tmp);

public slots:
    void    recgpsstore(QVariantList recstore);
    void    reccalcufinish(bool tmp);
    void    recOutOfRangeAngle(double tmp);
    void    qfirepointrec(QVariant tmp);
private:
    FireDetection*          firedetection;                      //FireDetection线程
    QList<QPoint>           square_store;                       //选定点的集合
    QmlObjectListModel      _fireLines;                         //传到missioneditor 自动画出线长
    QVariantList            GPS_store;                          //计算后的GPS坐标
    QVariantList            linesectionlength_;                 //每段的线长
    bool                    drawfinish;                         //绘制完成标志位
    double                  sum;                                //总面积
    bool                    calcufinish_,firepoint_,pointmode_; //计算完成、火点模式、画点模式标志位
    double                  linelength;                         //总线长
    double                  outOfRangeAngle_;                   //画面中超过水平面的角度
    QVariantList            qfirepoint_;                        //火点经纬度坐标


};
#endif // FIREDETECTION_H
