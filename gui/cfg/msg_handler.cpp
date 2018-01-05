// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/cfg/msg_handler.cpp
//! @brief     Implements messageHandler.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "cfg/msg_handler.h"
#include <iostream>
#include <QMessageBox>
#include <QStatusBar>
#include <QApplication>
#include <QMainWindow>
#include <QtGlobal> // no auto rm

#ifndef QT_NO_DEBUG
#define context(ctx) " [" << ctx.function << "]"
#else
#define context(ctx) ""
#endif

//extern MainWin* gMainWin;

void messageHandler(QtMsgType type, QMessageLogContext const& ctx, rcstr msg) {
    switch (type) {
    case QtDebugMsg:
        std::cerr << ".... " << msg.toStdString() << /*context(ctx) <<*/ "\n" << std::flush;
        break;
// unavailable before Qt5.5
//    case QtInfoMsg:
//        std::cerr << "INFO " << msg.toStdString() << context(ctx) << "\n" << std::flush;
//        gMainWin->statusBar()->showMessage(msg, 5000);
//        break;
    case QtFatalMsg:
        std::cerr << "BUG! " << msg.toStdString() << context(ctx) << "\n" << std::flush;
        QMessageBox::critical(QApplication::activeWindow(), qAppName(),
                              "Sorry, you encountered a fatal bug.\n"
                              "The application will terminate.\n"
                              "Please report the following to the maintainer:\n"
                              "Error message:\n" + msg + "\n"
#ifndef QT_NO_DEBUG
                              "Context:\n" + ctx.function + "\n"
#endif
            );
        qApp->quit();
        break;
    case QtWarningMsg:
    default:
        if (msg.left(4)=="QXcb") {
            std::cerr << "QBUG " << msg.toStdString() << "\n" << std::flush;
        } else {
            std::cerr << "WARN " << msg.toStdString() << "\n" << std::flush;
            QMessageBox::warning(QApplication::activeWindow(), qAppName(), msg);
        }
        break;
    }
}
