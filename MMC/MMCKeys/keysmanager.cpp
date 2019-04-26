#include "keysmanager.h"
#include "QGCApplication.h"
#include "MMC/MMCMount/mmcmount.h"

MMCKey::MMCKey(int id, QObject *parent) : QObject(parent), _id(id)
{
    _timer = new QTimer(this);
    _timer->setInterval(100);
    connect(_timer, SIGNAL(timeout()), this, SLOT(onTimerOut()));
}

void MMCKey::setKey(bool key)
{
    if(key == _key) return;
    _key = key;
    emit keyChanged(_key);

    if(_key){
        emit press(this);
        _accumulatedTime = 1;
        _timer->start();
    }else{
        emit upspring(this);
        _timer->stop();
        if(_accumulatedTime < 5) emit click(this);
    }
}

void MMCKey::onTimerOut()
{
    _accumulatedTime++;
    emit longPress(this);
}

//------------------------------------------------------------------------------

KeysManager::KeysManager(QObject *parent) : QObject(parent)
{
    for(int i = 0; i < 5; i++){
        _key[i] = new MMCKey(i, this);
        connect(_key[i], SIGNAL(press(MMCKey*)),     this, SLOT(onPress(MMCKey*)));
        connect(_key[i], SIGNAL(upspring(MMCKey*)),  this, SLOT(onUpspring(MMCKey*)));
        connect(_key[i], SIGNAL(longPress(MMCKey*)), this, SLOT(onLongPress(MMCKey*)));
        connect(_key[i], SIGNAL(click(MMCKey*)),     this, SLOT(onClick(MMCKey*)));
    }

    /* an jian yu gong neng bang ding */
    /* 0-A 1-B 2-C 3-D 4-E */
    _keyId[4] = KEY_REC_OR_PHOTO;
}

void KeysManager::setKey(int id, bool key)
{
    if(id < 0 || id > 4) return;
    _key[id]->setKey(key);
}
/* ----------------------------------------- */
void KeysManager::onPress(MMCKey *key)
{
    switch (_keyId[key->id()]) {
    case KEY_REC_OR_PHOTO:
        break;
    case KEY_ZOOM_DOWN:
        break;
    case KEY_ZOOM_UP:
        break;
    default:
        break;
    }
}

void KeysManager::onUpspring(MMCKey *key)
{
    switch (_keyId[key->id()]) {
    case KEY_REC_OR_PHOTO:
        if(key->accumulatedTime() > 1)
            REC();
        break;
    case KEY_ZOOM_DOWN:
        break;
    case KEY_ZOOM_UP:
        break;
    default:
        break;
    }
}

void KeysManager::onLongPress(MMCKey *key)
{
    switch (_keyId[key->id()]) {
    case KEY_REC_OR_PHOTO:
        break;
    case KEY_ZOOM_DOWN:
        break;
    case KEY_ZOOM_UP:
        break;
    default:
        break;
    }
}

void KeysManager::onClick(MMCKey *key)
{
    switch (_keyId[key->id()]) {
    case KEY_REC_OR_PHOTO:
        photo();
        break;
    case KEY_ZOOM_DOWN:
        break;
    case KEY_ZOOM_UP:
        break;
    default:
        break;
    }
}
/* ----------------------------------------- */
void KeysManager::photo()
{
//    if(qgcApp()->toolbox()->multiVehicleManager()->activeVehicleAvailable()){
//        Vehicle* activeVehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();
//        if(activeVehicle && !activeVehicle->mountLost() && activeVehicle->currentMount() && activeVehicle->currentMount()->mountType() ==MountInfo::MOUNT_CUSTOM){
//            CustomMount* camMount = dynamic_cast<CustomMount*>(activeVehicle->currentMount());
//            camMount->doCameraTrigger();
//        }
//    }


    if(qgcApp()->toolbox()->multiVehicleManager()->activeVehicleAvailable()){
        Vehicle* activeVehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();
        if(activeVehicle){
            activeVehicle->doCameraTrigger();
        }
    }
}

void KeysManager::REC()
{
    if(qgcApp()->toolbox()->multiVehicleManager()->activeVehicleAvailable()){
        Vehicle* activeVehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();
        if(activeVehicle && !activeVehicle->mountLost() && activeVehicle->currentMount() && activeVehicle->currentMount()->mountType() ==MountInfo::MOUNT_CUSTOM){
            CustomMount* camMount = dynamic_cast<CustomMount*>(activeVehicle->currentMount());
            camMount->videoTape();
        }
    }
}

