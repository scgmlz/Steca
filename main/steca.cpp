//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      main/steca.cpp
//! @brief     Implements the main program
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

//! \mainpage Steca: the stress and texture calculator
//!
//! \par Homepage:
//!           http://apps.jcns.fz-juelich.de/steca
//! \par Repository:
//!           https://github.com/scgmlz/Steca

#include "core/session.h"
#include "manifest.h"
#include "gui/view/msg_handler.h"
#include "gui/dialogs/file_dialog.h"
#include "gui/mainwin.h"
#include "qcr/engine/console.h"
//#include "qcr/base/debug.h"

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"

#include <iostream>
#include <QApplication>
#include <QLoggingCategory>
#include <QStyleFactory>

namespace {

void exit_help()
{
    std::cout << APPLICATION_CLAIM << "\n\n"
              << "Usage: " << APPLICATION_NAME << " [options]\n\n"
              << "Options:\n"
              << "  -h  Print this message.\n"
              << "  -v  Print " << APPLICATION_NAME << " version.\n"
//              << "  -c  Read commands from console instead of starting the GUI.\n"
              << "  -p  Sets the file overwrite policy to 'panic'.\n"
              << "  -s  Sets the file overwrite policy to 'silent overwrite'.\n\n"
              << "Default behavior:\n"
              << "  File overwrite policy is 'ask for confirmation', unless -p or -s is chosen.\n";
    exit(0);
}

void exit_version()
{
    std::cout << APPLICATION_NAME << " version " << VERSION << "\n";
    exit(0);
}

} // namespace


int main(int argc, char* argv[]) {
    struct optparse options;
    optparse_init(&options, argv);
    int opt;
    if (argc>1 && QString(argv[1])=="--help")
        exit_help();
    if (argc>1 && QString(argv[1])=="--version")
        exit_version();
    while ((opt = optparse(&options, "hvps")) != -1) {
        switch (opt) {
        case 'h':
            exit_help();
        case 'v':
            exit_version();
        case 'p':
            setFileOverwritePolicy(file_dialog::eFileOverwritePolicy::PANIC);
            break;
        case 's':
            setFileOverwritePolicy(file_dialog::eFileOverwritePolicy::SILENT_OVERWRITE);
            break;
        case '?':
            std::cerr << "Unsupported option. Use '" APPLICATION_NAME " -h' for list of options\n";
            exit(-1);
        default:
            std::cerr << "Bug: impossible option\n";
            exit(-1);
        }
    }

    QString startupScript = "";
    if ((startupScript = optparse_arg(&options))!="" && optparse_arg(&options)) {
        std::cerr << "More than one command-line argument given\n";
        exit(-1);
    }

    QApplication app(argc, argv);

    app.setApplicationName(APPLICATION_NAME);
    app.setApplicationVersion(VERSION);
    app.setOrganizationName(ORGANIZATION_NAME);
    app.setOrganizationDomain(ORGANIZATION_DOMAIN);

#if defined(Q_OS_OSX)
    app.setStyle(QStyleFactory::create("Macintosh"));
#elif defined(Q_OS_WIN)
    app.setStyle(QStyleFactory::create("Fusion"));
#else
    app.setStyle(QStyleFactory::create("Fusion"));
#endif

    const QString logFileName = startupScript==""
        ? qApp->applicationName() + ".log"
        : startupScript + ".log";
    new Console(logFileName);
    QLoggingCategory::setFilterRules("*.debug=true\nqt.*.debug=false");
    qInstallMessageHandler(messageHandler);

    Session session;
    new MainWin(startupScript); // must be pointer, because it can be deleted by 'quit' trigger
    return app.exec();
}
