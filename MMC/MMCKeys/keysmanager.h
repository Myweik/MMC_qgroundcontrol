#ifndef KEYSMANAGER_H
#define KEYSMANAGER_H

#include <QObject>
#include <QTimer>
#include <QMap>

class MMCKey : public QObject
{
    Q_OBJECT
public:
    explicit MMCKey(int id, QObject *parent = nullptr);

    bool key    (){ return _key; }
    void setKey(bool key);
    int  id()    {return _id; }
    double accumulatedTime() { return (double)_accumulatedTime * 0.1; }

signals:
    void press(MMCKey* key);        //an xia
    void upspring(MMCKey* key);     //tan qi
    void longPress(MMCKey* key);    //chang an xin tiao
    void click(MMCKey* key);        //dian jie

    void keyChanged(bool key);

private slots:
    void onTimerOut();

private:
    QTimer* _timer = nullptr;
    bool _key = false;
    int  _id  = -1;

    int _accumulatedTime = 1;
};

class KeysManager : public QObject
{
    Q_OBJECT
    typedef enum KEY_TYPE
    {
        KEY_REC_OR_PHOTO = 1,    //lu xiang huo pai zhao
        KEY_ZOOM_DOWN,  //suo xiao
        KEY_ZOOM_UP,    //fang da
    } KEY_TYPE;

public:
    explicit KeysManager(QObject *parent = nullptr);

    void setKey(int id, bool key);

signals:

public slots:
    void onPress(MMCKey *key);
    void onUpspring(MMCKey* key);     //tan qi
    void onLongPress(MMCKey* key);    //chang an xin tiao
    void onClick(MMCKey* key);        //dian jie

private:
    void photo(); //pai zhao
    void REC();   //lu xiang

private:
    /* jian zhi dui */
    QMap<int, int> _keyId;
    /* 5 key */
    MMCKey* _key[5];
};

#endif // KEYSMANAGER_H
