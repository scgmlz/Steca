TARGET = STeCa2

TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT      += core gui

CONFIG  += thread rtti exceptions warn_on
CONFIG  += c++11
CONFIG(devel): DEFINES += "DEVEL=1"

SOURCES += \
    defs.cpp \
    main.cpp \
    app.cpp \
    mainwin.cpp \
    settings.cpp \
    action.cpp
HEADERS += \
    defs.h \
    app.h \
    mainwin.h \
    settings.h \
    action.h
