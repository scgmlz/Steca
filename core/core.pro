TARGET   = core
TEMPLATE = lib

CONFIG   = static thread stl exceptions c++11
CONFIG  += silent warn_on

QT -= core gui

HEADERS += \
    core.h

SOURCES += \
    core.cpp
