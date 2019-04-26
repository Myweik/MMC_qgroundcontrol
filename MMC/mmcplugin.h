#ifndef MMCPLUGIN_H
#define MMCPLUGIN_H

#include "qtavplayer/src/QmlVideoPlayer.h"

static void mmcRegisterPlugin(){
    VlcQmlVideoPlayer::registerPlugin();
}

#endif // MMCPLUGIN_H
