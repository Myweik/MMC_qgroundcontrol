QT += multimedia quick qml core
CONFIG += c++11

RESOURCES += \
    $$PWD/transceiver.qrc \

HEADERS += \
    $$PWD/transceivermanager.h \
    $$PWD/radioprovider.h \
    $$PWD/radiomember.h \

SOURCES += \
    $$PWD/transceivermanager.cpp \
    $$PWD/radioprovider.cpp \
    $$PWD/radiomember.cpp \



