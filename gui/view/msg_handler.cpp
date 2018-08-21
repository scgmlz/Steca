//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/msg_handler.cpp
//! @brief     Implements messageHandler.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "qcr/engine/console.h"
#include <QApplication>
#include <QMessageBox>
#include <QtGlobal> // no auto rm
#include <iostream>

#ifndef QT_NO_DEBUG
#define context(ctx) " [" << ctx.function << "]"
#else
#define context(ctx) ""
#endif

void messageHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg) {
    switch (type) {
    case QtDebugMsg:
        std::cerr << "## " << msg.toStdString() << "\n" << std::flush;
        gConsole->log("## " + msg);
        break;
// unavailable before Qt5.5 (ISSUE #36)
//    case QtInfoMsg:
//        std::cerr << "INFO " << msg.toStdString() << context(ctx) << "\n" << std::flush;
//        gGui->statusBar()->showMessage(msg, 5000);
//        break;
    case QtWarningMsg:
    default:
        if (msg.left(4)=="QXcb")
            return;
        std::cerr << "WARN " << msg.toStdString() << "\n" << std::flush;
        if (!qApp)
            return;
        qApp->restoreOverrideCursor();
        QMessageBox::warning(QApplication::activeWindow(), qAppName(), msg);
        gConsole->log("##WARN: " + msg);
        break;
    case QtFatalMsg:
        std::cerr << "BUG! " << msg.toStdString() << context(ctx) << "\n" << std::flush;
        if (!qApp)
            return;
        qApp->restoreOverrideCursor();
        QMessageBox::critical(QApplication::activeWindow(), qAppName(),
                              "Sorry, you encountered a fatal bug.\n"
                              "The application will terminate.\n"
                              "Please save the log file and inform the maintainer.\n\n"
                              "Error:\n" + msg + "\n"
#ifndef QT_NO_DEBUG
                              "Context:\n" + ctx.function + "\n"
#endif
            );
        gConsole->log("##FATAL: " + msg);
        qApp->quit();
        break;
    }
}
