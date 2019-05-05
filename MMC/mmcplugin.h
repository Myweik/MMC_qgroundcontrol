#ifndef MMCPLUGIN_H
#define MMCPLUGIN_H

#include "qtavplayer/src/QmlVideoPlayer.h"
#include "transceiver/transceivermanager.h"
//#include "mmintrin.h"
#include "mmctools.h"

static void mmcRegisterPlugin(){
    VlcQmlVideoPlayer::registerPlugin();
}

#endif // MMCPLUGIN_H
