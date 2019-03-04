//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/base/exception.h
//! @brief     Defines and implements class Exception. Also defines macro THROW.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QException>
#include <QString>

//! The sole exception type to be used in core and gui.

//! Throw this if the exception is to be catched, and
//!   the program is to recover and to issue a qWarning().
//! For fatal bugs do not throw this, but use qFatal().

class Exception : public QException {
public:
    Exception(const QString& msg) noexcept : msg_{msg} {}
    const QString& msg() const noexcept { return msg_; }
private:
    QString msg_;
};

//! This macro shall be used as the sole way to throw an Exception.
#define THROW(msg) { throw Exception{msg}; }

#endif // EXCEPTION_H
