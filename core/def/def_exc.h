// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/def/def_exc.h
//! @brief     Defines class Exception
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

// exceptions

#ifndef DEF_EXC_H
#define DEF_EXC_H

#include "typ/typ_str.h"
#include <QException>

// An exception that carries a message.
class Exception : public QException {
    CLASS(Exception)
    SUPER(QException) protected : Exception(rcstr msg, bool silent) noexcept;

public:
    Exception() noexcept;
    Exception(rcstr msg) noexcept;
    Exception(rc) noexcept;

    bool silent() const noexcept { return silent_; }
    rcstr msg() const noexcept { return msg_; }
    pcstr what() const noexcept;

    void setMsg(rcstr);

    Exception* clone() const;
    void raise() const;

protected:
    str msg_;
    QByteArray msg8bit_;
    bool silent_;
};

// raise an exception
#define THROW(msg) throw Exception(msg)
#define THROW_SILENT() throw Exception()

// run-time condition checking
#define RUNTIME_CHECK(test, msg) if (!(test)) THROW(msg)

#endif // DEF_EXC_H
