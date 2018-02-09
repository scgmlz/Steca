// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/exception.h
//! @brief     Defines class Exception
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QException>

//! The sole exception type used in this software.
class Exception : public QException {
public:
    Exception() = delete;
    Exception(const QString& msg) noexcept : msg_(msg) {}
    const QString& msg() const noexcept { return msg_; }
private:
    QString msg_;
};

// raise an exception
#define THROW(msg) throw Exception(msg)

#endif // EXCEPTION_H
