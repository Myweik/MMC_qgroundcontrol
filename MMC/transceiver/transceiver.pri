QT += multimedia quick qml core
CONFIG += c++11

RESOURCES += \
    $$PWD/transceiver.qrc \

HEADERS += \
    $$PWD/transceivermanager.h \
    $$PWD/radioprovider.h \
    $$PWD/radiomember.h \
    $$PWD/radiomemberbase.h \
    $$PWD/androidraduimember.h

SOURCES += \
    $$PWD/transceivermanager.cpp \
    $$PWD/radioprovider.cpp \
    $$PWD/radiomember.cpp \
    $$PWD/radiomemberbase.cpp \
    $$PWD/androidraduimember.cpp



