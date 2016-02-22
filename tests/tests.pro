TARGET   = tests
TEMPLATE = app

CONFIG  += thread rtti exceptions c++11
CONFIG  += silent warn_on

QT += testlib

SOURCES += \
    test_core_lib.cpp \
    test_core_array2d.cpp \
    tests_main.cpp \
    test_core_image.cpp \
    test_core_range.cpp \
    test_approx.cpp

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

HEADERS += \
    test_core_array2d.h \
    test_core_lib.h \
    test_core_image.h \
    test_core_range.h \
    test_approx.h
