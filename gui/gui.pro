TARGET   = STeCa2
TEMPLATE = app

CONFIG  += thread rtti exceptions c++11
CONFIG  += silent warn_on

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT      += core gui printsupport

HEADERS += \
    QCP/qcustomplot.h \
    app.h \
    defs.h \
    delegates.h \
    gui_helpers.h \
    mainwin.h \
    settings.h \
    panel/panel.h \
    panel/background.h \
    panel/diffractogram.h \
    panel/reflections.h \
    panel/reflectioninfo.h \
    panel/files.h \
    panel/normalization.h \
    split_files.h \
    split_reflections.h \
    split_image.h \
    split_diffractogram.h \
    session.h \
    panel/datasets.h \
    panel/datasetinfo.h \
    panel/dataset.h \
    models.h

SOURCES += \
    QCP/qcustomplot.cpp \
    app.cpp \
    delegates.cpp \
    gui_helpers.cpp \
    main.cpp \
    mainwin.cpp \
    settings.cpp \
    panel/panel.cpp \
    panel/background.cpp \
    panel/diffractogram.cpp \
    panel/normalization.cpp \
    panel/files.cpp \
    panel/reflectioninfo.cpp \
    panel/reflections.cpp \
    split_files.cpp \
    split_reflections.cpp \
    split_image.cpp \
    split_diffractogram.cpp \
    session.cpp \
    panel/datasets.cpp \
    panel/datasetinfo.cpp \
    panel/dataset.cpp \
    models.cpp

RESOURCES += \
    resources.qrc

DISTFILES += \
    stash.txt

# core library

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lcore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lcore
else:unix: LIBS += -L$$OUT_PWD/../core/ -lcore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/libcore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/libcore.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/core.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/core.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../core/libcore.a
