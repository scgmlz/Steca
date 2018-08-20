//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/console.h
//! @brief     Defines class Console
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QDateTime>
#include <QTextStream>
#include <stack>

extern class Console* gConsole; //!< global handle that points to _the_ Console.

//! Logs user actions, and executes console commands, based on a command registry.

//! To be instantiated exactly once, and to be accessed through the global handle gConsole.

class Console
{
public:
    Console();
    ~Console();
    Console(const Console&) = delete;

    QString learn(QString name, class QcrSettable*); //!< learns _not_ commands, but widget names!
    void forget(const QString& name);
    void pop();
    void readFile(const QString& fName);
    void call(const QString&);
    void commandsFromStack();  //!< needed by modal dialogs

    void log(QString) const;
    bool hasCommandsOnStack() const { return !commandLifo_.empty(); } //!< needed by modal dialogs

private:
    enum class Caller { gui, cli, stack, sys } caller_ { Caller::gui };
    enum class Result : int { ok, err, suspend };

    class CommandRegistry& registry() const { return *registryStack_.top(); }

    void readLine();
    Result executeLine(QString);

    QDateTime startTime_;
#ifdef Q_OS_WIN
	class QWinEventNotifier *notifier_;
#else
	class QSocketNotifier *notifier_;
#endif
    std::stack<class CommandRegistry*> registryStack_;
    std::deque<QString> commandLifo_;

    mutable int computingTime_ {0};
    mutable QTextStream log_;
};

#endif // CONSOLE_H
