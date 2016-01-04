TARGET   = STeCa2
TEMPLATE = app

CONFIG  += thread rtti exceptions c++11
CONFIG  += silent warn_on

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT      += core gui printsupport

HEADERS += \
    app.h \
    defs.h \
    dock_files.h \
    dock_info.h \
    mainwin.h \
    panel.h \
    panel_detector.h \
    panel_files.h \
    panel_normalization.h \
    settings.h \
    images.h \
    imageinfo.h \
    reflectioninfo.h \
    reflections.h \
    panel_images.h \
    panel_diffractogram.h \
    delegates.h \
    gui_helpers.h \
    ../3rd/qcustomplot.h \
    panel_background.h

SOURCES += \
    main.cpp \
    app.cpp \
    defs.cpp \
    dock_files.cpp \
    dock_info.cpp \
    mainwin.cpp \
    panel.cpp \
    panel_detector.cpp \
    panel_files.cpp \
    panel_normalization.cpp \
    settings.cpp \
    images.cpp \
    imageinfo.cpp \
    reflectioninfo.cpp \
    reflections.cpp \
    panel_images.cpp \
    panel_diffractogram.cpp \
    delegates.cpp \
    gui_helpers.cpp \
    ../3rd/qcustomplot.cpp \
    panel_background.cpp

RESOURCES += \
    resources.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lcore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lcore
else:unix: LIBS += -L$$OUT_PWD/../core/ -lcore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

DISTFILES += \
    stash.txt
