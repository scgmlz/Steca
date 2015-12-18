TARGET = STeCa2

TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT      += core gui

CONFIG  += thread rtti exceptions printsupport warn_on
CONFIG  += c++11
CONFIG(devel): DEFINES += "DEVEL=1"

SOURCES += \
    defs.cpp \
    main.cpp \
    app.cpp \
    mainwin.cpp \
    settings.cpp \
    action.cpp \
    panes/background.cpp \
    panes/detector.cpp \
    panes/diffractogram.cpp \
    panes/files.cpp \
    panes/imageinfo.cpp \
    panes/images.cpp \
    panes/normalization.cpp \
    panes/pane.cpp \
    panes/reflectioninfo.cpp \
    panes/reflections.cpp \
    3rd/qcustomplot.cpp
HEADERS += \
    defs.h \
    app.h \
    mainwin.h \
    settings.h \
    action.h \
    panes/background.h \
    panes/detector.h \
    panes/diffractogram.h \
    panes/files.h \
    panes/imageinfo.h \
    panes/images.h \
    panes/normalization.h \
    panes/pane.h \
    panes/reflectioninfo.h \
    panes/reflections.h \
    panes/panes.h \
    3rd/qcustomplot.h
