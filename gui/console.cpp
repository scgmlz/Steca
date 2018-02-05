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
#include <QDateTime>
#include <QFile>
#include <QSocketNotifier>

Console* gConsole; //!< global

Console::Console()
{
    notifier_ = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
    connect(notifier_, SIGNAL(activated(int)), this, SLOT(readLine()));
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
    QTextStream qterr(stderr);
    QString line = qtin.readLine();
    if (line=="lscmd") {
        for (auto it: setters_)
            qterr << "setter: " << it.first << "\n";
        for (auto it: actions_)
            qterr << "action: " << it.first << "\n";
        return;
    }
    if (line.contains('=')) {
        QStringList list = line.split('=');
        QString cmd = list[0];
        QString val = list[1];
        auto entry = setters_.find(cmd);
        if (entry==setters_.end()) {
            qterr << "setter '" << cmd << "' not found\n";
            return;
        }
        entry->second(val);
        return;
    }
    if (line.endsWith('!')) {
        QString cmd = line.left(line.count()-1);
        auto entry = actions_.find(cmd);
        if (entry==actions_.end()) {
            qterr << "action '" << cmd << "' not found\n";
            return;
        }
        entry->second();
        return;
    }
    emit(transmitLine(line));
}

void Console::registerSetter(const QString& name, std::function<void(const QString&)> setter) {
    qDebug() << "register setter" << name;
    if (setters_.find(name)!=setters_.end())
        qFatal(("Duplicate setter '"+name+"'").toLatin1());
    setters_[name] = setter;
}

void Console::deregisterSetter(const QString& name) {
    qDebug() << "deregis setter" << name;
    auto it = setters_.find(name);
    if (it==setters_.end())
        qFatal(("Cannot deregister setter '"+name+"'").toLatin1());
    setters_.erase(it);
}

void Console::registerAction(const QString& name, std::function<void()> action) {
    qDebug() << "register action" << name;
    if (actions_.find(name)!=actions_.end())
        qFatal(("Duplicate action '"+name+"'").toLatin1());
    actions_[name] = action;
}

void Console::log(const QString& line) {
    qDebug() << line;
    log_ << "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm::ss.zzz") + "]"
         << line << "\n";
    log_.flush();
}
