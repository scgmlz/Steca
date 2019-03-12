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

#include <QObject>
#include <QString>
#include <stack>

extern class Console* gConsole; //!< global handle that points to _the_ Console.

//! Global singleton that executes script and console commands.

//! This class is to be instantiated exactly once though this is not enforced by an
//! explicit singleton mechanism. After creation, the single instance can be accessed
//! through the global handle gConsole.
//!
//! Command execution is based on a registry. QWidgets and QActions, enhanced by
//! inheritance from QcrCommandable, register and unregister themselves using the
//! commands "learn" and "forget".

class Console : public QObject {
    Q_OBJECT
public:
    Console();
    ~Console();
    Console(const Console&) = delete;

    QString learn(const QString& name, class QcrCommandable*);
    void forget(const QString& name);

    void openModalDialog (const QString& name, QcrCommandable* widget);
    void closeModalDialog(const QString& name);

    void runScript(const QString& fName);
    void commandsFromStack();

    bool hasCommandsOnStack() const;
signals:
    void closeDialog(bool ok) const;

private:
    enum class Result : int //!< Used to inform commandsFromStack how to proceed
    { ok,                   //!< Proceed with next command from stack
      err,                  //!< Terminate stack execution
      suspend               //!< Suspend stack execution
    };
    std::stack<class CommandRegistry*> registryStack_;
    std::deque<QString> commandStack_;

    class CommandRegistry* registry() const { return registryStack_.top(); }
    void readCLI();
    Result wrappedCommand(const QString& command);
};

#endif // CONSOLE_H
