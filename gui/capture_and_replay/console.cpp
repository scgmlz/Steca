//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/capture_and_replay/console.cpp
//! @brief     Implements class Console
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "console.h"
#include "core/def/debug.h"
#include "core/typ/exception.h"
#include <QApplication> // tmp
#include <QDateTime>
#include <QDir> // tmp
#include <QSocketNotifier>

Console* gConsole; //!< global

//  ***********************************************************************************************
//! @class CommandRegistry

//! Holds console (= terminal = command-line) commands to be defined and executed by class Console.
class CommandRegistry {
public:
    CommandRegistry() = delete;
    CommandRegistry(const QString& _name) : name_(_name) {}
    QString learn(const QString&, CSettable*);
    void forget(const QString&);
    CSettable* find(const QString& name);
    void dump(QTextStream&);
    QString name() const { return name_; }
private:
    const QString name_;
    std::map<const QString, CSettable*> widgets_;
    std::map<const QString, int> numberedEntries_;
};

QString CommandRegistry::learn(const QString& name, CSettable* widget)
{
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
    //qDebug() << "registry " << name_ << " learns " << ret;
    if (widgets_.find(ret)!=widgets_.end())
        qFatal("Duplicate widget registry entry '%s'", ret.toLatin1().constData());
    widgets_[ret] = widget;
    return ret;
}

void CommandRegistry::forget(const QString& name)
{
    //qDebug() << "registry " << name_ << " forgets " << name;
    auto it = widgets_.find(name);
    if (it==widgets_.end())
        qFatal("Cannot deregister, there is no entry '%s' in the widget registry",
                name.toLatin1().constData());
    widgets_.erase(it);
}

CSettable* CommandRegistry::find(const QString& name)
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
}

//  ***********************************************************************************************
//! @class Console

Console::Console()
{
    gConsole = this;
    notifier_ = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
    connect(notifier_, &QSocketNotifier::activated, [this](int) { readLine(); });

    // start registry
    registryStack_.push(new CommandRegistry("main"));

    // start log
    auto* file = new QFile("Steca.log");
    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        qFatal("cannot open log file");
    log_.setDevice(file);
    log("# Steca session started");
}

Console::~Console()
{
    log("# Steca session ended");
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
    exec(line);
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
                QTextStream qterr(stderr);
                qterr << "unbalanced '['\n";
                return;
            }
            line = line.mid(i+1);
        }
        commandLifo_.push_back(line);
    }
    commandsFromStack();
}

void Console::commandsFromStack()
{
    while (!commandLifo_.empty()) {
        const QString line = commandLifo_.front();
        commandLifo_.pop_front();
        if (line=="@close")
            return;
        caller_ = Caller::stack;
        Result ret = exec(line);
        caller_ = Caller::gui;
        if (ret==Result::err) {
            commandLifo_.clear();
            log("# Emptied command stack upon error");
        } else if (ret==Result::suspend)
            return;
    }
    log("# Command stack executed");
}

//! Commands issued by the system (and not by the user nor a command file) should pass here
void Console::call(const QString& line)
{
    caller_ = Caller::sys;
    exec(line);
    caller_ = Caller::gui;
}

Console::Result Console::exec(QString line)
{
    QTextStream qterr(stderr);
    if (line[0]=='#')
        return Result::ok; // comment => nothing to do
    if (line[0]=='@') {
        log(line);
        QStringList list = line.mid(1).split(' ');
        QString cmd = list[0];
        if (cmd=="ls") {
            registryStack_.top()->dump(qterr);
        } else if (cmd=="push") {
            if (list.size()<2) {
                qterr << "command @push needs argument <name>\n";
                return Result::err;
            }
            registryStack_.push(new CommandRegistry(list[1]));
        } else if (cmd=="pop") {
            if (registryStack_.empty()) {
                qterr << "cannot pop: registry stack is empty\n";
                return Result::err;
            }
            delete registryStack_.top();
            registryStack_.pop();
        } else if (cmd=="file") {
            if (list.size()<2) {
                qterr << "command @file needs argument <file_name>\n";
                return Result::err;
            }
            readFile(list[1]);
        } else if (cmd=="close") {
            return Result::suspend;
        } else {
            qterr << "@ command " << cmd << " not known\n";
            return Result::err;
        }
        return Result::ok;
    }
    QStringList args = line.split(' ');
    QString cmd = args.takeFirst();
    if (args.size()<1) {
        qterr << "Missing argument to command '" << cmd << "'\n";
        return Result::err;
    }
    CSettable* f = registry().find(cmd);
    if (!f) {
        qterr << "Command '" << cmd << "' not found\n";
        return Result::err;
    }
    try {
        f->onCommand(args); // execute command
        return Result::ok;
    } catch (Exception &ex) {
        qterr << "Command '" << line << "' failed:\n" << ex.msg() << "\n";
    }
    return Result::err;
}

QString Console::learn(const QString& name, CSettable* widget)
{
    return registry().learn(name, widget);
}

void Console::forget(const QString& name)
{
    registry().forget(name);
}

void Console::log2(bool hadFocus, const QString& line)
{
    if (caller_==Caller::gui && !hadFocus)
        log("#: " + line);
    else
        log(line);
}

void Console::log(const QString& line)
{
    log_ << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm::ss.zzz")
         << " " << registry().name() << "]";
    if (caller_==Caller::gui)
        ;
    else if (caller_==Caller::stack)
        log_ << "#< ";
    else if (caller_==Caller::cli)
        log_ << "#: ";
    else if (caller_==Caller::sys)
        log_ << "#! ";
    else
        qFatal("invalid case");
    log_ << line << "\n";
    log_.flush();
}
