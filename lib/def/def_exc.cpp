/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "def_exc.h"
#include "test/tests.h"
//------------------------------------------------------------------------------

#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
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

//------------------------------------------------------------------------------
// eof
