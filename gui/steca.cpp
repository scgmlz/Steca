// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/steca.cpp
//! @brief     Implements the main program
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "../manifest.h"
#include "mainwin.h"
#include "cfg/msg_handler.h"
#include "session.h"
#include <tclap/CmdLine.h> // templated command line argument parser, in 3rdparty directory
//#include <unistd.h>
#include <QApplication>
#include <QStyleFactory>

const char* version =
#include "../VERSION"
    ;

class QMainWindow* gMainWin;
class Session* gSession;

int main(int argc, char* argv[]) {

    TCLAP::CmdLine cmd("Stress and texture calculator", ' ', version, true);
    cmd.parse(argc, argv);

    QApplication app(argc, argv);

    app.setApplicationName(APPLICATION_NAME);
    app.setApplicationVersion(version);
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

    gSession = Session::instance();

    gui::MainWin mainWin;
    gMainWin = &mainWin;
    mainWin.show();
    qDebug() /* qInfo() TODO restore */ << "Welcome to Steca";

    return app.exec();
}
