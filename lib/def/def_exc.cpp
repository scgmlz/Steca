// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/def/def_exc.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //


#include "def_exc.h"
#include "test/tests.h"

#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif

#ifdef TESTS
#include "def/def_gsl.h"
#endif

Exception::Exception(rcstr msg, bool silent) noexcept
: silent_(silent) {
  setMsg(msg);
}


Exception::Exception() noexcept
: Cls(EMPTY_STR, true) {
}

TEST("Exception()",
  CHECK_THROWS_AS(THROW("Hello"), Exception);
  CHECK_THROWS_AS(THROW_SILENT(), Exception);

  CHECK_NOTHROW(Exception());
  CHECK_THROWS_AS(Exception().raise(), Exception);

  scoped<Exception*> p(Exception().clone());
  CHECK_THROWS_AS(p->raise(), Exception);
)

Exception::Exception(rcstr msg) noexcept
: Cls(msg, false) {
}

TEST("Exception(\"Hi\")",
  try {
    THROW("Hi");
  } catch (Exception &e) {
    CHECK_EQ("Hi", e.msg());
    return;
  }

  CHECK(false); // not here
)

Exception::Exception(rc that) noexcept
: Cls(that.msg_) {
}

pcstr Exception::what() const noexcept {
  return msg8bit_.constData();
}

TEST("Exception(\"Hi\")",
  try {
    THROW("Hi");
  } catch (Exception &e) {
    CHECK_EQ("Hi", str(e.what()));
    return;
  }

  CHECK(false); // not here
)

void Exception::setMsg(rcstr s) {
  msg_     = s;
  msg8bit_ = msg_.toLocal8Bit();
}

Exception* Exception::clone() const {
  return new Exception(*this);
}

void Exception::raise() const {
  throw *this;
}


