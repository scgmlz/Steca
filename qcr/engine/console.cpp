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
    if (my_match.size()!=7) {
        std::cerr << "BUG: invalid match size\n";
        exit(-1);
    }
    context = QString{my_match[4].str().c_str()};
    command = QString{my_match[5].str().c_str()}.trimmed();
    return true;
}

} // namespace

#ifndef LOCAL_CODE_ONLY

#include "qcr/engine/console.h"
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

//  ***********************************************************************************************
//! @class CommandRegistry

//! Holds console (= terminal = command-line) commands to be defined and executed by class Console.
class CommandRegistry {
public:
    CommandRegistry() = delete;
    CommandRegistry(const CommandRegistry&) = delete;
    CommandRegistry(const QString& _name) : name_{_name} {}
    QString learn(const QString&, QcrCommandable*);
    void forget(const QString&);
    QcrCommandable* find(const QString& name);
    void dump(QTextStream&) const;
    QString name() const { return name_; }
    int size() const { return widgets_.size(); }
private:
    const QString name_;
    std::map<const QString, QcrCommandable*> widgets_;
    std::map<const QString, int> numberedEntries_;
};

QString CommandRegistry::learn(const QString& name, QcrCommandable* widget)
{
    ASSERT(name!=""); // empty name only allowed for non-settable QcrBase
    // qterr << "Registry " << name_ << " learns '" << name << "'\n"; qterr.flush();
    QString ret = name;
    if (ret.contains("#")) {
        auto numberedEntry = numberedEntries_.find(name);
        int idxEntry = 1;
        if (numberedEntry==numberedEntries_.end())
            numberedEntries_[name] = idxEntry;
        else
            idxEntry = ++(numberedEntry->second);
        ret.replace("#", QString::number(idxEntry));
    }
    if (widgets_.find(ret)!=widgets_.end()) {
        QByteArray tmp = ret.toLatin1();
        qFatal("Duplicate widget registry entry '%s'", tmp.constData());
    }
    widgets_[ret] = widget;
    return ret;
}

void CommandRegistry::forget(const QString& name)
{
    // qterr << "Registry " << name_ << "(" << widgets_.size() << ") forgets '"  << name << "'\n";
    // qterr.flush();
    auto it = widgets_.find(name);
    if (it==widgets_.end()) {
        QByteArray tmp1 = name.toLatin1();
        QByteArray tmp2 = name_.toLatin1();
        qFatal("Cannot deregister, there is no entry '%s' in the widget registry '%s'",
               tmp1.constData(), tmp2.constData());
    }
    widgets_.erase(it);
}

QcrCommandable* CommandRegistry::find(const QString& name)
{
    auto entry = widgets_.find(name);
    if (entry==widgets_.end())
        return {};
    return entry->second;
}

void CommandRegistry::dump(QTextStream& stream) const
{
    for (auto it: widgets_)
        stream << " " << it.first;
    stream << "\n";
}

//  ***********************************************************************************************
//! @class Console

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
    caller_ = Caller::log;
    log("# " + qApp->applicationName() + " " + qApp->applicationVersion() + " started at "
        + startTime_.toString("yyyy-MM-dd HH:mm::ss.zzz"));
    caller_ = Caller::ini;
}

Console::~Console()
{
    caller_ = Caller::log;
    log("# " + qApp->applicationName() + " session ended");
    log("# duration: " + QString::number(startTime_.msecsTo(QDateTime::currentDateTime())) + "ms");
    log("# computing time: " + QString::number(computingTime_) + "ms");
    delete log_.device();
    while (!registryStack_.empty()) {
        delete registryStack_.top();
        registryStack_.pop();
    }
    gConsole = nullptr;
}

//! Registers a QcrCommandable or pushes a new registry.

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
        if (args[0]!="@push") {
            QByteArray tmp = name.toLatin1();
            qFatal("invalid @ command in learn(%s)", tmp.constData());
        }
        if (args.size()<2) {
            QByteArray tmp = name.toLatin1();
            qFatal("@push has no argument in learn(%s)", tmp.constData());
        }
        name = args[1];
        registryStack_.push(new CommandRegistry{name});
        // qterr << "pushed registry " << registryStack_.top()->name() << "\n";
        // qterr.flush();
    }
    return registry().learn(name, widget);
}

//! Unregisters a QcrCommandable.
void Console::forget(const QString& name)
{
    // qterr << "DEBUG: forget " << name << "\n";
    registry().forget(name);
}

//! Sets calling context to GUI. To be called when initializations are done.
void Console::startingGui()
{
    caller_ = Caller::gui;
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
        if (line[0]=='[') {
            int i = line.indexOf(']');
            if (i==-1) {
                qterr << "unbalanced '['\n"; qterr.flush();
                return;
            }
            line = line.mid(i+1);
        }
        commandLifo_.push_back(line);
    }
    commandsFromStack();
    log("# done with script '" + fName + "'");
}

//! Pops the current registry away, so that the previous one is reinstated.

//! Called by ~QcrModal(), i.e. on terminating a modal dialog.
void Console::closeModalDialog()
{
    log("@close # modal dialog");
    if (registryStack_.empty()) {
        qterr << "cannot pop: registry stack is empty\n"; qterr.flush();
        return;
    }
    // qterr << "going to pop registry " << registryStack_.top()->name() << "\n"; qterr.flush();
    delete registryStack_.top();
    registryStack_.pop();
    // qterr << "top registry is now " << registryStack_.top()->name() << "\n"; qterr.flush();
}

//! Executes commands on stack. Called by runScript and by QcrModalDialog/QcrFileDialog::exec.
void Console::commandsFromStack()
{
    while (!commandLifo_.empty()) {
        const QString line = commandLifo_.front();
        qterr << "DEBUG: command from stack: " << line << "\n";
        commandLifo_.pop_front();
        if (line=="@close")
            return;
        Result ret = commandInContext(line, Caller::fil);
        if (ret==Result::err) {
            commandLifo_.clear();
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
    auto currTime = QDateTime::currentDateTime();
    int tDiff = lastTime.msecsTo(currTime);
    lastTime = currTime;
    QString prefix = "[";
    if (caller_==Caller::gui && line[0]!='#') {
        prefix += "       "; // direct user action: we don't care how long the user was idle
    } else {
        prefix += QString::number(tDiff).rightJustified(5) + "ms";
        computingTime_ += tDiff;
    }
    prefix += " " + registry().name() + " " + callerCode() + "] ";
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
    return !commandLifo_.empty();
}

//! Returns three-letter code that indicates which kind of call caused the command to be logged.
QString Console::callerCode() const
{
    if      (caller_==Caller::log)
        return "log";
    else if (caller_==Caller::gui)
        return "gui";
    else if (caller_==Caller::ini)
        return "ini";
    else if (caller_==Caller::fil)
        return "fil";
    else if (caller_==Caller::cli)
        return "cli";
    else
        qFatal("BUG in Console::callerCode: invalid case");
}

//! Reads one line from the command-line interface, and executes it.
void Console::readCLI()
{
    QTextStream qtin(stdin);
    QString line = qtin.readLine();
    commandInContext(line, Caller::cli);
}

//! Delegates command execution to wrappedCommand, with context set to callerArg.
Console::Result Console::commandInContext(const QString& line, Caller callerArg)
{
    caller_ = callerArg;
    Result ret = wrappedCommand(line);
    caller_ = Caller::gui; // restores default
    return ret;
}

//! Executes command. Always called from commandInContext(..).

//! Commands are either console commands (starting with '@'), or widget commands.
//! Widget commands start with the name of widget that has been registered by learn(..);
//! further execution is delegated to the pertinent widget.
Console::Result Console::wrappedCommand(const QString& line)
{
    QString command, context;
    if (!parseCommandLine(line, command, context)) {
        qterr << "command line '" << line << "' could not be parsed\n"; qterr.flush();
        return Result::err;
    }
    if (command=="")
        return Result::ok; // nothing to do
    QString cmd, arg;
    strOp::splitOnce(command, cmd, arg);
    qterr << "DEBUG: wrapped command: " << command << "\n";
    if (cmd[0]=='@') {
        if (cmd=="@ls") {
            const CommandRegistry* reg = registryStack_.top();
            qterr << "registry " << reg->name() << " has " << reg->size() << " commands:\n";
            reg->dump(qterr);
            qterr.flush();
        } else if (cmd=="@close") {
            log(command);
            return Result::suspend;
        } else {
            qterr << "@ command " << cmd << " not known\n"; qterr.flush();
            return Result::err;
        }
        return Result::ok;
    }
    QcrCommandable* w = registry().find(cmd);
    if (!w) {
        qterr << "Command '" << cmd << "' not found\n"; qterr.flush();
        return Result::err;
    }
    try {
        w->setFromCommand(arg); // execute command
        return Result::ok;
    } catch (const QcrException&ex) {
        qterr << "Command '" << command << "' failed:\n" << ex.msg() << "\n"; qterr.flush();
    }
    return Result::err;
}

#endif // LOCAL_CODE_ONLY
