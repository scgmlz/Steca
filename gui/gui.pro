TARGET   = STeCa2
TEMPLATE = app

CONFIG  += thread rtti exceptions c++11
CONFIG  += silent warn_on

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT      += core gui printsupport

HEADERS += \
    ../3rd/qcustomplot.h \
    app.h \
    defs.h \
    delegates.h \
    gui_helpers.h \
    mainwin.h \
    settings.h \
    panels/panel.h \
    panels/background.h \
    panels/detector.h \
    panels/diffractogram.h \
    panels/reflections.h \
    panels/reflectioninfo.h \
    panels/files.h \
    panels/imageinfo.h \
    panels/normalization.h \
    panels/image.h \
    split_files.h \
    split.h \
    split_reflections.h \
    split_images.h \
    split_image.h \
    split_diffractogram.h \
    session.h \
    panels/datasets.h

SOURCES += \
    ../3rd/qcustomplot.cpp \
    app.cpp \
    defs.cpp \
    delegates.cpp \
    gui_helpers.cpp \
    main.cpp \
    mainwin.cpp \
    settings.cpp \
    panels/panel.cpp \
    panels/background.cpp \
    panels/detector.cpp \
    panels/diffractogram.cpp \
    panels/imageinfo.cpp \
    panels/normalization.cpp \
    panels/files.cpp \
    panels/reflectioninfo.cpp \
    panels/reflections.cpp \
    panels/image.cpp \
    split_files.cpp \
    split.cpp \
    split_reflections.cpp \
    split_images.cpp \
    split_image.cpp \
    split_diffractogram.cpp \
    session.cpp \
    panels/datasets.cpp

RESOURCES += \
    resources.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lcore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lcore
else:unix: LIBS += -L$$OUT_PWD/../core/ -lcore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

DISTFILES += \
    stash.txt
