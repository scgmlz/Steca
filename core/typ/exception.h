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

// exceptions

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "typ/str.h"
#include <QException>

// An exception that carries a message.
class Exception : public QException {
private:
public:
    Exception() noexcept;
    Exception(rcstr msg) noexcept;
    Exception(Exception const&) noexcept;

    bool silent() const noexcept { return silent_; }
    rcstr msg() const noexcept { return msg_; }
    const char* what() const noexcept;

    void setMsg(rcstr);

    Exception* clone() const;
    void raise() const;

protected:
    Exception(rcstr msg, bool silent) noexcept;
    str msg_;
    QByteArray msg8bit_;
    bool silent_;
};

// raise an exception
#define THROW(msg) throw Exception(msg)
#define THROW_SILENT() throw Exception()

// run-time condition checking
#define RUNTIME_CHECK(test, msg) if (!(test)) THROW(msg)

#endif // EXCEPTION_H
