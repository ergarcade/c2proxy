QT += core websockets
QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

CONFIG += debug_and_release
CONFIG(debug, debug|release) {
    # DEFINES += CSAFE_DEBUG DUMP_PACKETS BATCH_JSON
    DEFINES += BATCH_JSON
    TARGET = build/cli-c2proxy/cli-c2proxy-debug
    OBJECTS_DIR = build/objs-debug
    MOC_DIR = build/objs-debug
} else {
    DEFINES += QT_NO_DEBUG_OUTPUT
    DEFINES += BATCH_JSON
    TARGET = build/cli-c2proxy/cli-c2proxy-release
    OBJECTS_DIR = build/objs-release
    MOC_DIR = build/objs-release
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# From http://www.eliasing.com/qt/hid-qt
macx:  SOURCES += hid/mac/hid.c
unix: !macx:  SOURCES += hid/linux/hid-libusb.c
win32: SOURCES += hid/windows/hid.cpp

macx: LIBS += -framework CoreFoundation -framework IOkit
unix: !macx: LIBS += -lusb-1.0
win32: LIBS += -lSetupAPI

INCLUDEPATH += csafe include hid

SOURCES += \
    csafe/csafe.cpp \
    cli/main.cpp \
    proxy/pm.cpp \
    shared/util.cpp

HEADERS += \
    csafe/csafe.h \
    hid/hidapi.h \
    include/pm.h \
    include/util.h
