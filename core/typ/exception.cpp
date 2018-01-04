// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/exception.cpp
//! @brief     Implements class Exception
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "exception.h"
#include <QtGlobal> // otherwise, strange bugs result (file loading fails silently, ...)

#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif

Exception::Exception(rcstr msg, bool silent) noexcept : silent_(silent) { setMsg(msg); }

Exception::Exception() noexcept : Exception("", true) {}

Exception::Exception(rcstr msg) noexcept : Exception(msg, false) {}

Exception::Exception(Exception const& that) noexcept : Exception(that.msg_) {}

const char* Exception::what() const noexcept { return msg8bit_.constData(); }

void Exception::setMsg(rcstr s) {
    msg_ = s;
    msg8bit_ = msg_.toLocal8Bit();
}

Exception* Exception::clone() const { return new Exception(*this); }

void Exception::raise() const { throw *this; }
