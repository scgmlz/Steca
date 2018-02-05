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

class CommandRegistry {
public:
    void learn(const QString&, std::function<void(const QString&)>);
    void forget(const QString&);
    std::function<void(const QString&)>* find(const QString& name);
    void dump(QTextStream&);
private:
    std::map<const QString, std::function<void(const QString&)>> commands_;
};

void CommandRegistry::learn(const QString& name, std::function<void(const QString&)> f) {
    qDebug() << "register command" << name;
    if (commands_.find(name)!=commands_.end())
        qFatal(("Duplicate command '"+name+"'").toLatin1());
    commands_[name] = f;
}

void CommandRegistry::forget(const QString& name) {
    qDebug() << "deregis command" << name;
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

    // start log
    auto* file = new QFile("Steca.log");
    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        qFatal("cannot open log file");
    log_.setDevice(file);
    log("# Steca session started");

    // start registry
    registryStack_.push(new CommandRegistry);
}

Console::~Console() {
    log("# Steca session ended");
}

void Console::readLine()
{
    QTextStream qtin(stdin);
    QTextStream qterr(stderr);
    QString line = qtin.readLine();
    if (line=="lscmd") {
        registryStack_.top()->dump(qterr);
        return;
    }
    if (line.endsWith('!'))
        line = line.left(line.count()-1) + "=void";
    if (line.contains('=')) {
        QStringList list = line.split('=');
        QString cmd = list[0];
        QString val = list[1];
        std::function<void(const QString&)>* f = registryStack_.top()->find(cmd);
        if (!f) {
            qterr << "command '" << cmd << "' not found\n";
            return;
        }
        (*f)(val); // execute command
        return;
    }
    emit(transmitLine(line));
}

void Console::learn(const QString& name, std::function<void(const QString&)> f) {
    registryStack_.top()->learn(name, f);
}

void Console::forget(const QString& name) {
    registryStack_.top()->forget(name);
}

void Console::log(const QString& line) {
    qDebug() << line;
    log_ << "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm::ss.zzz") + "]"
         << line << "\n";
    log_.flush();
}
