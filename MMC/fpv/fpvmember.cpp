#include "fpvmember.h"

FpvMember::FpvMember(QObject *parent) : QObject(parent)
{
#if defined(Q_OS_ANDROID) //"ttysWK1"
    _fpvSerial = new fpvDataMutual("ttysWK3");
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN32)

#endif

}
