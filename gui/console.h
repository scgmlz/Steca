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
#include <map>
#include <functional>
#include <QObject>
#include <QTextStream>

extern class Console* gConsole; //!< global

//! Reads commands from stdin, and emits signal transmitLine
class Console : public QObject, public ISingleton<Console>
{
    Q_OBJECT
public:
    Console();
    ~Console();
    void registerSetter(const QString& name, std::function<void(const QString&)> setter);
    void deregisterSetter(const QString& name);
    void registerAction(const QString& name, std::function<void()> action);
    void log(const QString&);
signals:
    void transmitLine(str);
private:
    QTextStream log_;
    class QSocketNotifier *notifier_;
    std::map<const QString, std::function<void(const QString&)>> setters_;
    std::map<const QString, std::function<void()>> actions_;
private slots:
    void readLine();
};

#endif // CONSOLE_H
