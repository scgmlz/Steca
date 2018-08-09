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

extern class Console* gConsole; //!< global

//! Reads commands from stdin, and emits signal transmitLine
class Console
{
public:
    Console();
    ~Console();
    QString learn(QString name, class QcrSettable*);
    void forget(const QString& name);
    void readFile(const QString& fName);
    void call(const QString&);
    bool hasCommandsOnStack() const { return !commandLifo_.empty(); }
    void commandsFromStack();
    void log(QString);
    void log2(bool, const QString&);
private:
    class CommandRegistry& registry() { return *registryStack_.top(); }
    void readLine();
    QDateTime startTime_;
    int computingTime_ {0};
    QTextStream log_;
    enum class Caller { gui, cli, stack, sys } caller_ { Caller::gui };
    enum class Result : int { ok, err, suspend };
    Result exec(QString);
#ifdef Q_OS_WIN
	class QWinEventNotifier *notifier_;
#else
	class QSocketNotifier *notifier_;
#endif
    std::stack<class CommandRegistry*> registryStack_;
    std::deque<QString> commandLifo_;
};

#endif // CONSOLE_H
