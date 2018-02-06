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
#include <QDateTime>
#include <QFile>
#include <QSocketNotifier>

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

void Console::readLine()
{
    QTextStream qtin(stdin);
    QString line = qtin.readLine();
    qDebug() << "READ " << line;
    commandExec(line);
    qDebug() << "DONE " << line;
}

void Console::command(QString line) {
    qDebug() << "CMD " << line;
    commandExec(line);
    qDebug() << "DON " << line;
}

void Console::commandExec(QString line) {
    QTextStream qterr(stderr);
    if (line[0]=='[') {
        int i = line.indexOf(']');
        if (i==-1) {
            qterr << "unbalanced '['\n";
            return;
        }
        line = line.mid(i+1);
    }
    if (line[0]=='#')
        return; // comment => nothing to do
    if (line[0]=='@') {
        QStringList list = line.mid(1).split(' ');
        QString cmd = list[0];
        if (cmd=="ls") {
            registryStack_.top()->dump(qterr);
        } else if (cmd=="push") {
            if (list.size()<2)
                qterr << "command @push needs argument <name>\n";
            else
                registryStack_.push(new CommandRegistry(list[1]));
        } else if (cmd=="pop") {
            if (registryStack_.empty())
                qterr << "cannot pop: registry stack is empty\n";
            else
                registryStack_.pop();
        } else {
            qterr << "@ command " << cmd << " not known\n";
        }
        return;
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
            return;
        }
        (*f)(val); // execute command
        return;
    }
    qterr << "invalid command '" << line << "'\n";
}

void Console::learn(const QString& name, std::function<void(const QString&)> f) {
    registry().learn(name, f);
}

void Console::forget(const QString& name) {
    registry().forget(name);
}

void Console::log(const QString& line) {
    qDebug() << line;
    log_ << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm::ss.zzz")
         << " " << registry().name() << "]"
         << line << "\n";
    log_.flush();
}
