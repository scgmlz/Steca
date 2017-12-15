// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/steca.cpp
//! @brief     Implements the main program.
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "../manifest.h"
#include "mainwin.h"
#include <tclap/CmdLine.h> // templated command line argument parser, in 3rdparty directory
#include <QApplication>
#include <QMessageBox>
#include <QStatusBar>
#include <QStyleFactory>

static QMainWindow* pMainWin;

static void messageHandler(QtMsgType type, QMessageLogContext const& ctx, rcstr msg) {
    switch (type) {
    case QtDebugMsg:
        std::cerr << ".... " << msg.toStdString() << " [" << ctx.function << "]\n";
        break;
    case QtInfoMsg:
        std::cerr << "INFO " << msg.toStdString() << " [" << ctx.function << "]\n";
        pMainWin->statusBar()->showMessage(msg, 5000);
        break;
    case QtFatalMsg:
        std::cerr << "BUG! " << msg.toStdString() << " [" << ctx.function << "]\n";
        QMessageBox::critical(QApplication::activeWindow(), qAppName(),
                              "Sorry, you encountered a fatal bug.\n"
                              "The application will terminate.\n"
                              "Please report the following to the maintainer:\n"
                              "Error message:\n" + msg + "\n"
                              "Context:\n" + ctx.function + "\n");
        qApp->quit();
        break;
    case QtWarningMsg:
    default:
        if (msg.left(4)=="QXcb") {
            std::cerr << "QBUG " << msg.toStdString() << "\n";
        } else {
            std::cerr << "WARN " << msg.toStdString() << " [" << ctx.function << "]\n";
            QMessageBox::warning(QApplication::activeWindow(), qAppName(), msg);
        }
        break;
    }
}

int main(int argc, char* argv[]) {

    TCLAP::CmdLine cmd("Stress and texture calculator", ' ',
#include "../VERSION"
                       , true);
    cmd.parse(argc, argv);

    QApplication app(argc, argv);

    app.setApplicationName(APPLICATION_NAME);
    app.setApplicationVersion(
#include "../VERSION"
        );
    app.setOrganizationName(ORGANIZATION_NAME);
    app.setOrganizationDomain(ORGANIZATION_DOMAIN);

#if defined(Q_OS_OSX)
    app.setStyle(QStyleFactory::create("Macintosh"));
#elif defined(Q_OS_WIN)
    app.setStyle(QStyleFactory::create("Fusion"));
#else
    app.setStyle(QStyleFactory::create("Fusion"));
#endif

    qInstallMessageHandler(messageHandler);

    gui::MainWin mainWin;
    pMainWin = &mainWin;
    mainWin.show();
    qInfo() << "Welcome to Steca";

    return app.exec();
}
