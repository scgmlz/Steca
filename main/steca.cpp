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

#include "clara.hpp"

#include <iostream>
#include <QApplication>
#include <QLoggingCategory>
#include <QStyleFactory>

namespace {

void exit_help()
{
    std::cout << APPLICATION_CLAIM << "\n\n"
              << "Usage: " << APPLICATION_NAME << " [options] [startup_script]\n\n"
              << "Options:\n"
              << "  -h        Print this message.\n"
              << "  -v        Print " << APPLICATION_NAME << " version.\n"
              << "  -l <file> Write log to <file>.\n"
//            << "  -c        Read commands from console instead of starting the GUI.\n"
              << "  -p        Sets the file overwrite policy to 'panic'.\n"
              << "  -s        Sets the file overwrite policy to 'silent overwrite'.\n\n"
              << "Default behavior:\n"
              << "  Log file name is <startup_script>.log or " << APPLICATION_NAME << ".log\n"
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
    QString logFileName;
    QString startupScript;

    bool help = false;
    bool vers = false;
    std::string theLogFileName {};
    std::string startupScriptSource {};
    bool panic = false;
    bool silent_overwrite = false;
    auto parser = clara::detail::Opt(help)["-h"]["--help"]("Show the help message.")|
                  clara::detail::Opt(vers)["-v"]["--version"]("Print version.")|
                  clara::detail::Opt(theLogFileName, "file")["-l"]("Write log to <file>.")|
                  clara::detail::Opt(panic)["-p"]("Sets the file overwrite policy to 'panic'.")|
                  clara::detail::Opt(silent_overwrite)["-s"]("Sets the file overwrite policy to 'silent overwrite'.")|
                  clara::detail::Arg(startupScriptSource, "file")("The path of the startup skript.");
    try {
        auto result = parser.parse(clara::detail::Args(argc, argv));
        if (!result) {
            std::cerr << "Unsuported option or missing option argument.\n"
                      << "Use '" APPLICATION_NAME " -h' for list of options\n";
            exit(-1);
        } else if (help)
            exit_help();
        else if (vers)
            exit_version();
        else if (theLogFileName != "")
            logFileName = QString::fromStdString(theLogFileName);
        else if (panic)
            setFileOverwritePolicy(file_dialog::eFileOverwritePolicy::PANIC);
        else if (silent_overwrite)
            setFileOverwritePolicy(file_dialog::eFileOverwritePolicy::SILENT_OVERWRITE);
        startupScript = QString::fromStdString(startupScriptSource);
    } catch (std::exception const & e) {
        std::cerr << e.what();
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

    if (logFileName=="") {
        logFileName = startupScript==""
            ? qApp->applicationName() + ".log"
            : startupScript + ".log";
    } else {
        if (logFileName==startupScript) {
            std::cerr << "Log file name coincides with startup script name\n";
            exit(-1);
        }
    }
    std::cout << "Log file will be written to " << logFileName.toLatin1().constData() << "\n";
    Console console(logFileName);
    QLoggingCategory::setFilterRules("*.debug=true\nqt.*.debug=false");
    qInstallMessageHandler(messageHandler);

    Session session;
    new MainWin{startupScript}; // must be pointer, because it can be deleted by 'quit' trigger
    return app.exec();
}
