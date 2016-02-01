TARGET   = core
TEMPLATE = lib
CONFIG  += staticlib

CONFIG  += thread rtti exceptions c++11
CONFIG  += silent warn_on

QT      += core

HEADERS += \
    loaders/Caress/car_datatypes.h \
    loaders/Caress/car_globals.h \
    loaders/Caress/raw.h \
    loaders/core_loadcaress.h \
    core_file.h \
    core_dataset.h \
    core_image.h \
    core_debug.h \
    core_defs.h \
    core_session.h

SOURCES += \
    loaders/Caress/raw.cpp \
    loaders/Caress/ReadCaress.cpp \
    loaders/core_loadcaress.cpp \
    core_dataset.cpp \
    core_defs.cpp \
    core_file.cpp \
    core_image.cpp \
    core_session.cpp

DISTFILES += \
    README \
    loaders/Caress/README
