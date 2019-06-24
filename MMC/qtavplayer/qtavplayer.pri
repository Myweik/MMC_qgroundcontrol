QT += multimedia quick qml core  usb
CONFIG += c++11

SOURCES += \
    $$PWD/src/avdecoder.cpp \
    $$PWD/src/AVThread.cpp \
    $$PWD/src/QmlVideoObject.cpp \
    $$PWD/src/QmlVideoPlayer.cpp \
    $$PWD/src/painter/GlPainter.cpp \
    $$PWD/src/painter/GlslPainter.cpp \
    $$PWD/src/Enums.cpp \
    $$PWD/src/usbexample.cpp \
    $$PWD/src/fifo.cpp

HEADERS += \
    $$PWD/src/avdecoder.h \
    $$PWD/src/AVThread.h \
    $$PWD/src/AVDefine.h \
    $$PWD/src/AVMediaCallback.h \
    $$PWD/src/QmlVideoObject.h \
    $$PWD/src/QmlVideoPlayer.h \
    $$PWD/src/painter/GlPainter.h \
    $$PWD/src/painter/GlslPainter.h \
    $$PWD/src/Enums.h \
    $$PWD/src/usbexample.h \
    $$PWD/src/fifo.h

RESOURCES += \
    $$PWD/ffmpegqml.qrc

unix{
    # Android and iOS don't unclude these
LIBS += -L$$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/ -lavcodec -lavfilter -lavformat -lavutil -lswscale -lswresample
#-lavdevice -lpostproc
INCLUDEPATH += $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/include

 contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/libs/android/libusb-1.0.22/android/libs/armeabi-v7a/libusb1.0.so\
        $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libavcodec.so \
        $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libavfilter.so \
        $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libavutil.so \
        $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libavformat.so \
        $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libswresample.so \
        $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libswscale.so

#        $$PWD/libs/android/libusb-1.0.22/android/libs/armeabi-v7a/libusb1.0.so\
#        $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libavcodec-57.so \
#$$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libavdevice-57.so \
#        $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libavfilter-6.so \
#        $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libavutil-55.so \
#        $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libavformat-57.so \
#$$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libpostproc-54.so \
#        $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libswresample-2.so \
#        $$PWD/libs/android/ffmpeg-4.0.2-android-gcc-lite/lib/armeabi-v7a/libswscale-4.so \
#        $$PWD/libs/android/openssl/libcrypto.so \
#        $$PWD/libs/android/openssl/libssl.so \
#        $$PWD/libs/android/openssl/libcrypto.so.1.0.0 \
#        $$PWD/libs/android/openssl/libssl.so.1.0.0 \
    }
}


win32 {
LIBS += -L$$PWD/libs/lib/win32/ -lavcodec -lavfilter -lavformat -lavutil -lswresample -lswscale
INCLUDEPATH += $$PWD/libs/include
DEPENDPATH += $$PWD/libs/include
include(libusb0/libusb0.pri)
}

