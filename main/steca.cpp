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

const char* version =
#include "../VERSION"
    ;

int main(int argc, char* argv[]) {
    struct optparse options;
    optparse_init(&options, argv);
    int opt;
    while ((opt = optparse(&options, "hvcps")) != -1) {
        switch (opt) {
        case 'h':
            std::cout << APPLICATION_CLAIM << "\n\n"
                      << "Usage: " << APPLICATION_NAME << " [options]\n\n"
                      << "Options:\n"
                      << "  -h  Print this message.\n"
                      << "  -v  Print " << APPLICATION_NAME << " version.\n"
                      << "  -c  Read commands from console instead of starting the GUI.\n"
                      << "  -p  Sets the file overwrite policy to 'Panic'. Default is 'Promt'.\n"
                      << "  -s  Sets the file overwrite policy to 'Silent Overwrite'. Default is 'Promt'.\n";
            exit(0);
        case 'v':
            std::cout << APPLICATION_NAME << " version " << version << "\n";
            exit(0);
        case 'p':
            setFileOverwritePolicy(file_dialog::eFileOverwritePolicy::PANIC);
            break;
        case 's':
            setFileOverwritePolicy(file_dialog::eFileOverwritePolicy::SILENT_OVERWRITE);
            break;
        }
    }

    QStringList nonoptArgs;
    const char* tmp;
    while ((tmp = optparse_arg(&options)))
        nonoptArgs.append(tmp);
    if (nonoptArgs.size()>1) {
        std::cerr << "More than one command-line argument given\n";
        exit(-1);
    }

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

    Console console;
    QLoggingCategory::setFilterRules("*.debug=true\nqt.*.debug=false");
    qInstallMessageHandler(messageHandler);

    Session session;
    new MainWin; // must be pointer, because it can be deleted by 'quit' trigger
    if (nonoptArgs.size())
        gConsole->call("@file " + nonoptArgs[0]);
    app.exec();
}
