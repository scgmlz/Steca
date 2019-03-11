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

#include <regex>
#include <iostream>
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
    const std::regex my_regex("^(\\[\\s*((\\d+)ms)?\\s*(\\w+)\\s\\w{3}\\])?([^#]*)(#.*)?$");
    std::smatch my_match;
    const std::string tmpLine { line.toLatin1().constData() };
    if (!std::regex_match(tmpLine, my_match, my_regex))
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
#include "qcr/engine/command_registry.h"
#include "qcr/engine/mixin.h"
#include "qcr/base/qcrexception.h"
#include "qcr/base/string_ops.h"
#include "qcr/base/debug.h" // ASSERT
#include <QApplication>
#include <QFile>

#ifdef Q_OS_WIN
#include <QWinEventNotifier>
#include <windows.h>
#else
#include <QSocketNotifier>
#endif

Console* gConsole; //!< global

QTextStream qterr(stderr);


Console::Console(const QString& logFileName)
{
    gConsole = this;

#ifdef Q_OS_WIN
    notifier_ = new QWinEventNotifier;// GetStdHandle(STD_INPUT_HANDLE));
    QObject::connect(notifier_, &QWinEventNotifier::activated, [this] (HANDLE) {readCLI(); });
#else
    notifier_ = new QSocketNotifier{fileno(stdin), QSocketNotifier::Read};
    QObject::connect(notifier_, &QSocketNotifier::activated, [this](int) { readCLI(); });
#endif

    // start registry
    registryStack_.push(new CommandRegistry{"main"});

    // start log
    auto* file = new QFile{logFileName};
    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        qFatal("cannot open log file");
    log_.setDevice(file);
    startTime_ = QDateTime::currentDateTime();
    caller_ = "log";
    log("# " + qApp->applicationName() + " " + qApp->applicationVersion() + " started at "
        + startTime_.toString("yyyy-MM-dd HH:mm::ss.zzz"));
    caller_ = "ini";
}

Console::~Console()
{
    caller_ = "log";
    log("# " + qApp->applicationName() + " session ended");
    log("# duration: " + QString::number(startTime_.msecsTo(QDateTime::currentDateTime())) + "ms");
    log("# computing time: " + QString::number(computingTime_) + "ms");
    delete log_.device();
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
QString Console::learn(const QString& nameArg, QcrCommandable* widget)
{
    QString name = nameArg;
    if (name[0]=='@') {
        QStringList args = name.split(' ');
        if (args[0]!="@push")
            qFatal("BUG: invalid @ command in learn(%s)", CSTRI(name));
        if (args.size()<2)
            qFatal("BUG: @push has no argument in learn(%s)", CSTRI(name));
        name = args[1];
        registryStack_.push(new CommandRegistry{name});
        // qDebug() << "pushed registry " << registry()->name();
    }
    return registry()->learn(name, widget);
}

//! Unregisters a QcrCommandable.
void Console::forget(const QString& name)
{
    // qDebug() << "DEBUG: forget " << name;
    registry()->forget(name);
}

//! Sets calling context to GUI. To be called when initializations are done.
void Console::startingGui()
{
    caller_ = "gui";
}

//! Reads and executes a command script.
void Console::runScript(const QString& fName)
{
    QFile file(fName);
    log("# running script '" + fName + "'");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file " << fName;
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        commandStack_.push_back(line);
    }
    commandsFromStack();
    log("# done with script '" + fName + "'");
}

//! Pops the current registry away, so that the previous one is reinstated.

//! Called by ~QcrModal(), i.e. on terminating a modal dialog.
void Console::closeModalDialog()
{
    log("@close # modal dialog");
    if (registryStack_.empty())
        qFatal("BUG or invalid @close command: cannot pop, registry stack is empty");
    // qDebug() << "going to pop registry " << registry->name();
    delete registryStack_.top();
    registryStack_.pop();
    // qDebug() << "top registry is now " << registry()->name();
}

//! Executes commands on stack. Called by runScript and by QcrModalDialog/QcrFileDialog::exec.
void Console::commandsFromStack()
{
    while (!commandStack_.empty()) {
        const QString line = commandStack_.front();
        qDebug() << "command from stack: " << line;
        commandStack_.pop_front();
        if (line=="@close")
            return;
        caller_ = "scr";
        Result ret = wrappedCommand(line);
        caller_ = "gui"; // restores default
        if (ret==Result::err) {
            commandStack_.clear();
            log("# Emptied command stack upon error");
            return;
        } else if (ret==Result::suspend)
            return;
    }
}

//! Writes line to log file, decorated with information on context and timing.
void Console::log(const QString& lineArg) const
{
    QString line = lineArg;
    static auto lastTime = startTime_;
    const auto currTime = QDateTime::currentDateTime();
    int tDiff = lastTime.msecsTo(currTime);
    lastTime = currTime;
    QString prefix = "[";
    if (caller_=="gui" && line[0]!='#') {
        prefix += "       "; // direct user action: we don't care how long the user was idle
    } else {
        prefix += QString::number(tDiff).rightJustified(5) + "ms";
        computingTime_ += tDiff;
    }
    prefix += " " + registry()->name() + " " + caller_ + "] ";
    log_ << prefix << line << "\n";
    log_.flush();
    if (line.indexOf("##")!=0) {
        qterr << line << "\n";
        qterr.flush();
    }
}

//! Returns true if there are commands on stack. Needed by modal dialogs.
bool Console::hasCommandsOnStack() const
{
    return !commandStack_.empty();
}

//! Reads one line from the command-line interface, and executes it.
void Console::readCLI()
{
    QTextStream qtin(stdin);
    QString line = qtin.readLine();
    caller_ = "cli";
    wrappedCommand(line);
    caller_ = "gui"; // restores default
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
    qDebug() << "wrapped command: " << command;
    if (cmd[0]=='@') {
        if (cmd=="@ls") {
            const CommandRegistry* reg = registry();
            qterr << "registry " << reg->name() << " has " << reg->size() << " commands:\n";
            reg->dump(qterr);
            qterr.flush();
        } else if (cmd=="@close") {
            log(command);
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
