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
    m_notifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
    connect(m_notifier, SIGNAL(activated(int)), this, SLOT(readLine()));
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
        auto entry = setters.find(cmd);
        if (entry==setters.end()) {
            qterr << "setter '" << cmd << "' not found\n";
            return;
        }
        entry->second(val);
        return;
    }
    emit(transmitLine(line));
}

void Console::registerSetter(
    const QString& name, std::function<void(const QString&)> setter) {
    setters[name] = setter;
}
