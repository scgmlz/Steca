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
    core_file.h \
    core_dataset.h \
    core_image.h \
    core_debug.h \
    core_defs.h \
    core_session.h \
    core_lib.h \
    loaders/core_loaders.h \
    core_array2d.h \
    loaders/Mar/mar300_header.h \
    loaders/Mar/mar345_header.h \
    loaders/Mar/MarReader.h \
    LevMar/levmar.h \
    LevMar/compiler.h \
    LevMar/misc.h \
    LevMar/FastDelegate/FastDelegate.h \
    LevMar/FastDelegate/FastDelegateBind.h \
    approx/approx_functions.h \
    approx/approx_methods.h

SOURCES += \
    loaders/Caress/raw.cpp \
    loaders/Caress/ReadCaress.cpp \
    core_dataset.cpp \
    core_defs.cpp \
    core_file.cpp \
    core_image.cpp \
    core_session.cpp \
    core_lib.cpp \
    loaders/core_loaders.cpp \
    core_array2d.cpp \
    LevMar/Axb.cpp \
    LevMar/Axb_core.cpp \
    LevMar/lm.cpp \
    LevMar/lmbc.cpp \
    LevMar/lmbc_core.cpp \
    LevMar/lmblec.cpp \
    LevMar/lmblec_core.cpp \
    LevMar/lmbleic.cpp \
    LevMar/lmbleic_core.cpp \
    LevMar/lm_core.cpp \
    LevMar/lmlec.cpp \
    LevMar/lmlec_core.cpp \
    LevMar/misc.cpp \
    LevMar/misc_core.cpp \
    approx/approx_functions.cpp \
    approx/approx_methods.cpp

DISTFILES += \
    README \
    loaders/Caress/README
