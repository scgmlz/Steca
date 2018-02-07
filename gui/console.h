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
#include <deque>
#include <stack>
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
    void learn(const QString& name, std::function<void(const QString&)> setter);
    void forget(const QString& name);
    void log(const QString&);
    void readFile(const QString& fName);
    void commandFromStack();
    void command(const QString&);
    class CommandRegistry& registry() { return *registryStack_.top(); }
private:
    void exec(QString);
    QTextStream log_;
    class QSocketNotifier *notifier_;
    std::stack<class CommandRegistry*> registryStack_;
    std::deque<QString> commandLifo_;
private slots:
    void readLine();
};

#endif // CONSOLE_H
