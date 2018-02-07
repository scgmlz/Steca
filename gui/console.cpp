// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/console.cpp
//! @brief     Implements class Console
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/console.h"
#include "core/def/debug.h"
#include <map>
#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QSocketNotifier>
#include <QTimer>

Console* gConsole; //!< global

// ************************************************************************** //
//  class CommandRegistry
// ************************************************************************** //

//! Holds console (= terminal = command-line) commands to be defined and executed by class Console.
class CommandRegistry {
public:
    CommandRegistry() = delete;
    CommandRegistry(const QString& _name) : name_(_name) {}
    void learn(const QString&, std::function<void(const QString&)>);
    void forget(const QString&);
    std::function<void(const QString&)>* find(const QString& name);
    void dump(QTextStream&);
    QString name() const { return name_; }
private:
    const QString name_;
    std::map<const QString, std::function<void(const QString&)>> commands_;
};

void CommandRegistry::learn(const QString& name, std::function<void(const QString&)> f) {
    qDebug() << "learn command" << name;
    if (commands_.find(name)!=commands_.end())
        qFatal(("Duplicate command '"+name+"'").toLatin1());
    commands_[name] = f;
}

void CommandRegistry::forget(const QString& name) {
    qDebug() << "forget command" << name;
    auto it = commands_.find(name);
    if (it==commands_.end())
        qFatal(("Cannot deregister command '"+name+"'").toLatin1());
    commands_.erase(it);
}

std::function<void(const QString&)>* CommandRegistry::find(const QString& name) {
    auto entry = commands_.find(name);
    if (entry==commands_.end())
        return {};
    return &entry->second;
}

void CommandRegistry::dump(QTextStream& stream) {
    stream << "commands:\n";
    for (auto it: commands_)
        stream << " " << it.first << "\n";
}

// ************************************************************************** //
//  class Console
// ************************************************************************** //

namespace {
/* unused
const char* to_s(Console::Caller caller) {
    switch (caller) {
    case gui: return "GUI";
    case cli: return "CLI";
    case stack: return "stack";
    case sys: return "sys";
    }
}
*/
} // namespace

Console::Console()
{
    notifier_ = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
    connect(notifier_, SIGNAL(activated(int)), this, SLOT(readLine()));

    // start registry
    registryStack_.push(new CommandRegistry("main"));

    // start log
    auto* file = new QFile("Steca.log");
    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        qFatal("cannot open log file");
    log_.setDevice(file);
    log("# Steca session started");
}

Console::~Console() {
    log("# Steca session ended");
}

void Console::readLine() {
    QTextStream qtin(stdin);
    QString line = qtin.readLine();
    qDebug() << "READ " << line;
    caller_ = Caller::cli;
    exec(line);
    caller_ = Caller::gui;
    qDebug() << "DONE " << line;
}

void Console::readFile(const QString& fName) {
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
    log("# Put file " + fName + " on command stack");
    commandsFromStack();
}

void Console::commandsFromStack() {
    while (!commandLifo_.empty()) {
        const QString line = commandLifo_.front();
        commandLifo_.pop_front();
        qDebug() << "ST> " << line;
        if (line=="@close")
            return;
        log("FROM STACK> " + line);
        caller_ = Caller::stack;
        Result ret = exec(line);
        caller_ = Caller::gui;
        qDebug() << "ST< " << line << " -> " << static_cast<int>(ret);
        if (ret==Result::err) {
            commandLifo_.clear();
            log("# Emptied command stack upon error");
        } else if (ret==Result::suspend)
            return;
    }
    log("# Command stack executed");
}

//! Commands issued by the system (and not by the user nor a command file) should pass here
void Console::call(const QString& line) {
    qDebug() << "CMD " << line;
    caller_ = Caller::sys;
    exec(line);
    caller_ = Caller::gui;
    qDebug() << "DON " << line;
}

Console::Result Console::exec(QString line) {
    QTextStream qterr(stderr);
    if (line[0]=='#')
        return Result::ok; // comment => nothing to do
    if (line[0]=='@') {
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
    if (line.endsWith('!'))
        line = line.left(line.count()-1) + "=void";
    if (line.contains('=')) {
        QStringList list = line.split('=');
        QString cmd = list[0];
        QString val = list[1];
        std::function<void(const QString&)>* f = registry().find(cmd);
        if (!f) {
            qterr << "command '" << cmd << "' not found\n";
            return Result::err;
        }
        (*f)(val); // execute command
        return Result::ok;
    }
    qterr << "invalid command '" << line << "'\n";
    return Result::err;
}

void Console::learn(const QString& name, std::function<void(const QString&)> f) {
    registry().learn(name, f);
}

void Console::forget(const QString& name) {
    registry().forget(name);
}

void Console::log2(bool hadFocus, const QString& line) {
    if (hadFocus)
        log("2: " + line); // TODO rm 2:
    else
        log("#: " + line);
}

void Console::log(const QString& line) {
    qDebug() << "LOG:" << line;
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
