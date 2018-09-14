//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/misc/exception.h
//! @brief     Defines class Exception
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

//! Throw this if the exception is to be catched, and the program can recover.
//! For fatal bugs do not throw this, but use qFatal().

class Exception : public QException {
public:
    Exception(const QString& msg) noexcept;
    const QString& msg() const noexcept { return msg_; }
private:
    QString msg_;
};

// The sole way to throw an Exception.
// TODO reconsider whether output is needed here, and whether to prefer std::cerr over qDebug
#define THROW(msg) { throw Exception(msg); }

#endif // EXCEPTION_H
