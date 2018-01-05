// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/console.cpp
//! @brief     Implements class Console
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "console.h"
#include "def/debug.h"
#include <iostream>
#include <QSocketNotifier>
#include <QTextStream>

Console::Console()
{
    m_notifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
    connect(m_notifier, SIGNAL(activated(int)), this, SLOT(readLine()));
}

void Console::readLine()
{
    QTextStream qtin(stdin);
    emit(transmitLine(qtin.readLine()));
}
