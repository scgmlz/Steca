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
    Console() = delete;
    Console(const QString& logFileName);
    ~Console();
    Console(const Console&) = delete;

    QString learn(const QString& name, class QcrCommandable*);
    void forget(const QString& name);
    void closeModalDialog();
    void runScript(const QString& fName);
    void commandsFromStack();
    void startingGui();

    void log(const QString&) const;
    bool hasCommandsOnStack() const;
private:
    enum class Caller { log,   //!< log entries come from console
                        gui,   //!< commands come from user action in GUI
                        ini,   //!< commands come from MainWin initialization
                        cli,   //!< commands come from command-line interface
                        fil    //!< commands come from stack, hence from file
    } caller_;
    enum class Result : int { ok, err, suspend };
    QDateTime startTime_;
#ifdef Q_OS_WIN
    class QWinEventNotifier *notifier_;
#else
    class QSocketNotifier *notifier_;
#endif
    std::stack<class CommandRegistry*> registryStack_;
    std::deque<QString> commandLifo_;
    mutable int computingTime_ {0}; //!< Accumulated computing time in ms.
    mutable QTextStream log_;

    QString callerCode() const;
    class CommandRegistry& registry() const { return *registryStack_.top(); }
    void readCLI();
    Result commandInContext(const QString&, Caller);
    Result wrappedCommand(const QString&);
};

#endif // CONSOLE_H
