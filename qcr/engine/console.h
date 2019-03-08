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

//! Global singleton that logs user actions, and executes script and console commands.

//! This class is to be instantiated exactly once though this is not enforced by an
//! explicit singleton mechanism. After creation, the single instance can be accessed
//! through the global handle gConsole.
//!
//! Command execution is based on a registry. QWidgets and QActions, enhanced by
//! inheritance from QcrCommandable, register and unregister themselves using the
//! commands "learn" and "forget".

class Console
{
public:
    Console() = delete;
    Console(const QString& logFileName);
    ~Console();
    Console(const Console&) = delete;

    QString learn(const QString& name, class QcrCommandable*);
    void forget(const QString& name);

    void startingGui();
    void runScript(const QString& fName);
    void closeModalDialog();
    void commandsFromStack();

    void log(const QString&) const;
    bool hasCommandsOnStack() const;
private:
    QString caller_;
    enum class Result : int //!< Used to inform commandsFromStack how to proceed
    { ok,                   //!< Proceed with next command from stack
      err,                  //!< Terminate stack execution
      suspend               //!< Suspend stack execution
    };
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

    class CommandRegistry& registry() const { return *registryStack_.top(); }
    void readCLI();
    Result commandInContext(const QString& command, const QString& caller);
    Result wrappedCommand(const QString& command);
};

#endif // CONSOLE_H
