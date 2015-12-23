TARGET   = core
TEMPLATE = lib

DEFINES += BUILDING_LIBRARY

CONFIG  += thread rtti stl exceptions c++11
CONFIG  += silent warn_on

QT -= gui

HEADERS += \
    core.h

SOURCES += \
    core.cpp

DISTFILES += \
    README
