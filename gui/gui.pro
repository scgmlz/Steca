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
    layout_helpers.h \
    mainimagewidget.h \
    mainwin.h \
    panel.h \
    panel_detector.h \
    panel_files.h \
    panel_normalization.h \
    settings.h

SOURCES += \
    main.cpp \
    app.cpp \
    defs.cpp \
    dock_files.cpp \
    dock_info.cpp \
    layout_helpers.cpp \
    mainimagewidget.cpp \
    mainwin.cpp \
    panel.cpp \
    panel_detector.cpp \
    panel_files.cpp \
    panel_normalization.cpp \
    settings.cpp

RESOURCES += \


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lcore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lcore
else:unix: LIBS += -L$$OUT_PWD/../core/ -lcore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core
