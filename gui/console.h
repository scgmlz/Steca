// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/console.h
//! @brief     Declares class Console
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CONSOLE_H
#define CONSOLE_H

#include "core/typ/str.h"
#include "core/typ/singleton.h"
#include <QObject>
#include <map>
#include <functional>

extern class Console* gConsole; //!< global

//! Reads commands from stdin, and emits signal transmitLine
class Console : public QObject, public ISingleton<Console>
{
    Q_OBJECT
public:
    Console();
    void registerSetter(const QString& name, std::function<void(const QString&)> setter);
signals:
    void transmitLine(str);
private:
    class QSocketNotifier *m_notifier;
    std::map<const QString, std::function<void(const QString&)>> setters;
private slots:
    void readLine();
};

#endif // CONSOLE_H
