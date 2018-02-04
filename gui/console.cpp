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
#include <QSocketNotifier>
#include <QTextStream>

Console* gConsole; //!< global

Console::Console()
{
    notifier_ = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
    connect(notifier_, SIGNAL(activated(int)), this, SLOT(readLine()));
}

void Console::readLine()
{
    QTextStream qtin(stdin);
    QTextStream qterr(stderr);
    QString line = qtin.readLine();
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

void Console::registerSetter(
    const QString& name, std::function<void(const QString&)> setter) {
    setters_[name] = setter;
}

void Console::registerAction(
    const QString& name, std::function<void()> action) {
    actions_[name] = action;
}
