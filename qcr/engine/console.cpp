//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/console.cpp
//! @brief     Implements class Console
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "qcr/base/debug.h"
#include <regex>
#include <QString>

namespace {

//! Parses a command line, sets the command and the context, and returns true if successful.

//! The input line may be either a plain command or a log entry.
//! A log entry starts with a '[..]' comment containing execution time (optional) and context.
//! It may end with a '#..' comment.
//!
//! Covered by utest/qcr/local/10_console

bool parseCommandLine(const QString& line, QString& command, QString& context)
{
    const std::regex my_regex{"^(\\[\\s*((\\d+)ms)?\\s*(\\w+)\\s\\w{3}\\])?([^#]*)(#.*)?$"};
    std::smatch my_match;
    const std::string tmp = CSTRI(line);
    if (!std::regex_match(tmp, my_match, my_regex))
        return false;
    if (my_match.size()!=7)
        qFatal("BUG in parseCommandLine: invalid match size");
    context = QString{my_match[4].str().c_str()};
    command = QString{my_match[5].str().c_str()}.trimmed();
    return true;
}

} // namespace

#ifndef LOCAL_CODE_ONLY

#include "qcr/engine/console.h"
#include "qcr/engine/logger.h"
#include "qcr/engine/command_registry.h"
#include "qcr/engine/mixin.h"
#include "qcr/base/qcrexception.h"
#include "qcr/base/string_ops.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>

#ifdef Q_OS_WIN
#include <QWinEventNotifier>
#include <windows.h>
#else
#include <QSocketNotifier>
#endif

namespace {
QTextStream qterr{stderr};
} // namespace

Console* gConsole; //!< global


Console::Console()
{
    gConsole = this;

#ifdef Q_OS_WIN
    auto* notifier = new QWinEventNotifier;// GetStdHandle(STD_INPUT_HANDLE));
    QObject::connect(notifier, &QWinEventNotifier::activated, [this] (HANDLE) {readCLI(); });
#else
    auto* notifier = new QSocketNotifier{fileno(stdin), QSocketNotifier::Read};
    QObject::connect(notifier, &QSocketNotifier::activated, [this](int) { readCLI(); });
#endif

    registryStack_.push(new CommandRegistry{"main"});
}

Console::~Console()
{
    while (!registryStack_.empty()) {
        delete registry();
        registryStack_.pop();
    }
    gConsole = nullptr;
}

//! Registers a QcrCommandable or pushes a new registry; returns registered name.

//! The registered name will deviated from the name passed as argument if it contains
//! a "#" (which will be replaced by a unique number).
//!
//! The registry will be used in "wrappedCommand" which forwards a command to the
//! QcrCommandable which then executes it.
//!
//! In the special case of nameArg="@push <name>", a new registry is pushed to current.
//! This is used by the QcrModal modal dialogs. On terminating, QcrModal calls
//! closeModalDialog(), which pops the current registry away, so that the previous
//! registry is reinstated.
QString Console::learn(const QString& name, QcrCommandable* widget)
{
    return registry()->learn(name, widget);
}

//! Unregisters a QcrCommandable.
void Console::forget(const QString& name)
{
    // qDebug() << "DEBUG: forget " << name;
    registry()->forget(name);
}

void Console::openModalDialog(const QString& name, QcrCommandable* widget)
{
    registryStack_.push(new CommandRegistry{name});
    gLogger->setLevel(name);
    // qDebug() << "pushed registry " << registry()->name();
    ASSERT(registry()->learn(name, widget) == name); // no change of name allowed here
}

//! Pops the current registry away, so that the previous one is reinstated.

//! Called by ~QcrModal(), i.e. on terminating a modal dialog.
void Console::closeModalDialog(const QString& name)
{
    ASSERT(name == registry()->name());
    forget(name);
    if (registryStack_.empty())
        qFatal("BUG or invalid @close command: cannot pop, registry stack is empty");
    // qDebug() << "going to pop registry " << name;
    delete registry();
    registryStack_.pop();
    gLogger->setLevel(registry()->name());
    // qDebug() << "top registry is now " << registry()->name();
}

//! Reads and executes a command script.
void Console::runScript(const QString& fName)
{
    QFile file{fName};
    gLogger->log("# running script '" + fName + "'");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file " << fName;
        return;
    }
    QTextStream in{&file};
    while (!in.atEnd()) {
        QString line = in.readLine();
        commandStack_.push_back(line);
    }
    commandsFromStack();
    gLogger->log("# done with script '" + fName + "'");
}

//! Executes commands on stack. Called by runScript and by QcrModalDialog/QcrFileDialog::exec.
void Console::commandsFromStack()
{
    gLogger->setCaller("scr");
    while (!commandStack_.empty()) {
        const QString line = commandStack_.front();
        // qDebug() << "command from stack: " << line;
        commandStack_.pop_front();
        Result ret = wrappedCommand(line);
        if (ret==Result::err) {
            commandStack_.clear();
            gLogger->log("# Emptied command stack upon error");
            break;
        } else if (ret==Result::suspend)
            break;
    }
    gLogger->setCaller("gui"); // restores default
}

//! Returns true if there are commands on stack. Needed by modal dialogs.
bool Console::hasCommandsOnStack() const
{
    return !commandStack_.empty();
}

//! Reads one line from the command-line interface, and executes it.
void Console::readCLI()
{
    QTextStream qtin{stdin};
    QString line = qtin.readLine();
    gLogger->setCaller("cli");
    wrappedCommand(line);
    gLogger->setCaller("gui"); // restores default
}

//! Executes command. Always called from commandInContext(..).

//! Commands are either console commands (starting with '@'), or widget commands.
//! Widget commands start with the name of widget that has been registered by learn(..);
//! further execution is delegated to the pertinent widget.
Console::Result Console::wrappedCommand(const QString& line)
{
    QString command, context;
    if (!parseCommandLine(line, command, context)) {
        qWarning() << "command line '" << line << "' could not be parsed";
        return Result::err;
    }
    if (command=="")
        return Result::ok; // nothing to do
    QString cmd, arg;
    strOp::splitOnce(command, cmd, arg);
    // qDebug() << "registry=" << registry()->name() << " --- wrapped command: " << command;
    if (cmd[0]=='@') {
        if (cmd=="@ls") {
            const CommandRegistry* reg = registry();
            qterr << "registry " << reg->name() << " has " << reg->size() << " commands:\n";
            reg->dump(qterr);
            qterr.flush();
        } else if (cmd=="@accept" || cmd=="@reject") {
            if (registry()->name()=="main") {
                qWarning() << "Command " << cmd << " makes no sense at main level";
                return Result::err;
            }
            emit closeDialog(cmd=="@accept");
            return Result::suspend;
        } else {
            qWarning() << "@ command " << cmd << " not known";
            return Result::err;
        }
        return Result::ok;
    }
    QcrCommandable* w = registry()->find(cmd);
    if (!w) {
        qWarning() << "Command '" << cmd << "' not found";
        return Result::err;
    }
    try {
        w->setFromCommand(arg); // execute command
        return Result::ok;
    } catch (const QcrException&ex) {
        qWarning() << "Command '" << command << "' failed:\n" << ex.msg();
    }
    return Result::err;
}

#endif // LOCAL_CODE_ONLY
