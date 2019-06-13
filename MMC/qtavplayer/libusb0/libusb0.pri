HEADERS += \
#    $$PWD/fifo.h \
    $$PWD/hidapi.h \
    $$PWD/lusb0_usb.h \
    $$PWD/usbinterface.h \
#    $$PWD/usbmonitor.h \
    $$PWD/libusb.h

SOURCES += \
#    $$PWD/fifo.cpp \
    $$PWD/usbinterface.cpp \
#    $$PWD/usbmonitor.cpp \
    $$PWD/libusb_dyn.c \
    $$PWD/libusb.cpp
