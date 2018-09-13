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

#include "qcr/engine/console.h"
#include "qcr/engine/mixin.h"
#include "qcr/base/qcrexception.h"
#include "qcr/base/string_ops.h"
#include "qcr/base/debug.h" // ASSERT
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
    CommandRegistry(const QString& _name) : name_(_name) {}
    QString learn(const QString&, QcrSettable*);
    void forget(const QString&);
    QcrSettable* find(const QString& name);
    void dump(QTextStream&);
    QString name() const { return name_; }
private:
    const QString name_;
    std::map<const QString, QcrSettable*> widgets_;
    std::map<const QString, int> numberedEntries_;
};

QString CommandRegistry::learn(const QString& name, QcrSettable* widget)
{
    ASSERT(name!=""); // empty name only allowed for non-settable QcrMixin
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
    // qterr << "Registry " << name_ << " forgets '" << name << "'\n"; qterr.flush();
    auto it = widgets_.find(name);
    if (it==widgets_.end()) {
        QByteArray tmp1 = name.toLatin1();
        QByteArray tmp2 = name_.toLatin1();
        qFatal("Cannot deregister, there is no entry '%s' in the widget registry '%s'",
               tmp1.constData(), tmp2.constData());
    }
    widgets_.erase(it);
}

QcrSettable* CommandRegistry::find(const QString& name)
{
    auto entry = widgets_.find(name);
    if (entry==widgets_.end())
        return {};
    return entry->second;
}

void CommandRegistry::dump(QTextStream& stream)
{
    stream << "commands:\n";
    for (auto it: widgets_)
        stream << " " << it.first << "\n";
    stream.flush();
}

//  ***********************************************************************************************
//! @class Console

Console::Console()
{
    gConsole = this;

#ifdef Q_OS_WIN
	notifier_ = new QWinEventNotifier();// GetStdHandle(STD_INPUT_HANDLE));
	QObject::connect(notifier_, &QWinEventNotifier::activated, [this] (HANDLE) {readLine(); });
#else
	notifier_ = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read);
        QObject::connect(notifier_, &QSocketNotifier::activated, [this](int) { readLine(); });
#endif

    // start registry
    registryStack_.push(new CommandRegistry("main"));

    // start log
    auto* file = new QFile("Steca.log");
    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        qFatal("cannot open log file");
    log_.setDevice(file);
    startTime_ = QDateTime::currentDateTime();
    log("#  Steca started at " + startTime_.toString("yyyy-MM-dd HH:mm::ss.zzz"));
}

Console::~Console()
{
    log("#  Steca session ended");
    log("#  duration: " + QString::number(startTime_.msecsTo(QDateTime::currentDateTime())) + "ms");
    log("#  computing time: " + QString::number(computingTime_) + "ms");
    delete log_.device();
    while (!registryStack_.empty()) {
        delete registryStack_.top();
        registryStack_.pop();
    }
}

void Console::readLine()
{
    QTextStream qtin(stdin);
    QString line = qtin.readLine();
    caller_ = Caller::cli;
    executeLine(line);
    caller_ = Caller::gui;
}

void Console::readFile(const QString& fName)
{
    QFile file(fName);
    log("@file " + fName);
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
    log("# @file " + fName + " executed");
}

void Console::commandsFromStack()
{
    while (!commandLifo_.empty()) {
        const QString line = commandLifo_.front();
        commandLifo_.pop_front();
        if (line=="@close")
            return;
        caller_ = Caller::stack;
        Result ret = executeLine(line);
        caller_ = Caller::gui;
        if (ret==Result::err) {
            commandLifo_.clear();
            log("# Emptied command stack upon error");
        } else if (ret==Result::suspend)
            return;
    }
}

//! Commands issued by the system (and not by the user nor a command file) should pass here
void Console::call(const QString& line)
{
    caller_ = Caller::sys;
    executeLine(line);
    caller_ = Caller::gui;
}

Console::Result Console::executeLine(QString line)
{
    if (line[0]=='#')
        return Result::ok; // comment => nothing to do
    QString cmd, arg;
    strOp::splitOnce(line, cmd, arg);
    if (cmd[0]=='@') {
        log(line);
        if (cmd=="@ls") {
            qterr << "registry " << registryStack_.top()->name() << " has commands:\n";
            qterr.flush();
            registryStack_.top()->dump(qterr);
        } else if (cmd=="@file") {
            readFile(arg);
        } else if (cmd=="@close") {
            return Result::suspend;
        } else {
            qterr << "@ command " << cmd << " not known\n"; qterr.flush();
            return Result::err;
        }
        return Result::ok;
    }
    QcrSettable* f = registry().find(cmd);
    if (!f) {
        qterr << "Command '" << cmd << "' not found\n"; qterr.flush();
        return Result::err;
    }
    try {
        f->executeConsoleCommand(arg); // execute command
        return Result::ok;
    } catch (QcrException &ex) {
        qterr << "Command '" << line << "' failed:\n" << ex.msg() << "\n"; qterr.flush();
    }
    return Result::err;
}

QString Console::learn(QString name, QcrSettable* widget)
{
    if (name[0]=='@') {
        QStringList args = name.split(' ');
        if (args.size()<2) {
            QByteArray tmp = name.toLatin1();
            qFatal("invalid @ construct in learn(%s)", tmp.constData());
        }
        if (args[0]!="@push") {
            QByteArray tmp = name.toLatin1();
            qFatal("invalid @ command in learn(%s)", tmp.constData());
        }
        name = args[1];
        registryStack_.push(new CommandRegistry(name));
    }
    return registry().learn(name, widget);
}

void Console::pop()
{
    if (registryStack_.empty()) {
        qterr << "cannot pop: registry stack is empty\n"; qterr.flush();
        return;
    }
    //qterr << "going to pop registry " << registryStack_.top()->name() << "\n";
    //qterr.flush();
    delete registryStack_.top();
    registryStack_.pop();
    //qterr << "top registry is now " << registryStack_.top()->name() << "\n";
    //qterr.flush();
}

void Console::forget(const QString& name)
{
    registry().forget(name);
}

void Console::log(QString line) const
{
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
    prefix += " " + registry().name() + "]";
    if      (caller_==Caller::gui || caller_==Caller::stack)
        ; // no further embellishment
    else if (caller_==Caller::cli)
        line = "#c " + line;
    else if (caller_==Caller::sys)
        line = "#s " + line;
    else
        qFatal("invalid case");
    log_ << prefix << line << "\n";
    log_.flush();
    if (line.indexOf("##")!=0) {
        qterr << line << "\n";
        qterr.flush();
    }
}
