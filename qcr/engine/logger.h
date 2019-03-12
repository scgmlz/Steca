//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/logger.h
//! @brief     Defines class Logger
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef LOGGER_H
#define LOGGER_H

#include <QDateTime>
#include <QTextStream>
#include <stack>

extern class Logger* gLogger; //!< global handle that points to _the_ Logger.

//! Global singleton that logs user actions.

//! This class is to be instantiated exactly once though this is not enforced by an
//! explicit singleton mechanism. After creation, the single instance can be accessed
//! through the global handle gLogger.

class Logger
{
public:
    Logger() = delete;
    Logger(const QString& logFileName);
    ~Logger();
    Logger(const Logger&) = delete;

    void log(const QString&) const;
    QString caller() const { return caller_; } // TODO sometimes says "gui" instead of "scr"
    void setLevel (const QString& level ) { level_  = level ; }
    void setCaller(const QString& caller) { caller_ = caller; }

private:
    QString level_ {"main"};
    QString caller_;
    QDateTime startTime_;
    mutable QTextStream log_;
    mutable int computingTime_ {0}; //!< Accumulated computing time in ms.
};

#endif // LOGGER_H
