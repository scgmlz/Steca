// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/steca.cpp
//! @brief     Implements messageHandler.
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "msg_handler.h"
#include <iostream>
#include <QMessageBox>
#include <QStatusBar>
#include <QApplication>
#include <QMainWindow>

#ifndef QT_NO_DEBUG
#define context(ctx) " [" << ctx.function << "]"
#else
#define context(ctx) ""
#endif

//extern QMainWindow* pMainWin;

void messageHandler(QtMsgType type, QMessageLogContext const& ctx, rcstr msg) {
    switch (type) {
    case QtDebugMsg:
        std::cerr << ".... " << msg.toStdString() << context(ctx) << "\n" << std::flush;
        break;
    case QtInfoMsg:
        std::cerr << "INFO " << msg.toStdString() << context(ctx) << "\n" << std::flush;
        pMainWin->statusBar()->showMessage(msg, 5000);
        break;
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
            std::cerr << "WARN " << msg.toStdString()
                      << " [" << ctx.function << "]\n" << std::flush;
            QMessageBox::warning(QApplication::activeWindow(), qAppName(), msg);
        }
        break;
    }
}
