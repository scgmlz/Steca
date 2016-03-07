TARGET   = core
TEMPLATE = lib
CONFIG  += staticlib

CONFIG  += thread rtti exceptions c++11
CONFIG  += silent warn_on

QT      += core

HEADERS += \
    io/Caress/car_datatypes.h \
    io/Caress/car_globals.h \
    io/Caress/raw.h \
    core_file.h \
    core_dataset.h \
    core_image.h \
    core_debug.h \
    core_defs.h \
    core_session.h \
    core_array2d.h \
    io/Mar/mar300_header.h \
    io/Mar/mar345_header.h \
    io/Mar/MarReader.h \
    LevMar/levmar.h \
    LevMar/compiler.h \
    LevMar/misc.h \
    LevMar/FastDelegate/FastDelegate.h \
    LevMar/FastDelegate/FastDelegateBind.h \
    core_types.h \
    io/core_io.h \
    core_fit_functions.h \
    core_fit_methods.h \
    core_fit_fitting.h \
    core_fit_limits.h \
    core_reflection.h

SOURCES += \
    io/Caress/raw.cpp \
    io/Caress/ReadCaress.cpp \
    core_dataset.cpp \
    core_defs.cpp \
    core_file.cpp \
    core_image.cpp \
    core_session.cpp \
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
    core_types.cpp \
    io/core_io_caress.cpp \
    io/core_io_tiff.cpp \
    io/core_io.cpp \
    core_fit_functions.cpp \
    core_fit_methods.cpp \
    core_fit_fitting.cpp \
    core_reflection.cpp

DISTFILES += \
    io/Caress/README
