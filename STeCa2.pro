TARGET = STeCa

TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT      += core gui printsupport

CONFIG  += thread rtti exceptions warn_on
CONFIG  += c++11
CONFIG(devel): DEFINES += "DEVEL=1"

HEADERS += \
    manifest.h \
    defs.h \
    gui/app.h \
    gui/mainwin.h \
    gui/settings.h \
    panes/background.h \
    panes/diffractogram.h \
    panes/files.h \
    panes/imageinfo.h \
    panes/images.h \
    panes/pane.h \
    panes/reflectioninfo.h \
    panes/reflections.h \
    panes/panes.h \
    3rd/qcustomplot.h \
    panes/image.h \
    widgets/iconbutton.h \
    widgets/editcell.h \
    widgets/delegates.h \
    gui/mainimagewidget.h \
    gui/dock_info.h \
    gui/dock_files.h \
    gui/panel_files.h \
    gui/panel.h \
    gui/layout_helpers.h \
    gui/panel_detector.h \
    gui/panel_normalization.h

SOURCES += \
    main.cpp \
    defs.cpp \
    gui/app.cpp \
    gui/mainwin.cpp \
    gui/settings.cpp \
    panes/background.cpp \
    panes/diffractogram.cpp \
    panes/files.cpp \
    panes/imageinfo.cpp \
    panes/images.cpp \
    panes/pane.cpp \
    panes/reflectioninfo.cpp \
    panes/reflections.cpp \
    3rd/qcustomplot.cpp \
    panes/image.cpp \
    widgets/iconbutton.cpp \
    widgets/editcell.cpp \
    widgets/delegates.cpp \
    gui/mainimagewidget.cpp \
    gui/dock_files.cpp \
    gui/dock_info.cpp \
    gui/panel_files.cpp \
    gui/panel.cpp \
    gui/layout_helpers.cpp \
    gui/panel_detector.cpp \
    gui/panel_normalization.cpp

RESOURCES += \
    rsrc.qrc
