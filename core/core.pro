TARGET   = core
TEMPLATE = lib

DEFINES += BUILDING_LIBRARY

CONFIG  += thread rtti exceptions c++11
CONFIG  += silent warn_on

QT      += core

HEADERS += \
    core.h

SOURCES += \
    core.cpp

DISTFILES += \
    README
