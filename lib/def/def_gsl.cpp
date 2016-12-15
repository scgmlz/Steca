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

#include "def_gsl.h"
#include "test/tests.h"
//------------------------------------------------------------------------------

#ifdef TESTS
namespace {

TEST_CASE("not_null<>") {
  int i = 0;
  auto p1 = not_null<int*>::from(&i), p2(p1);
  CHECK_EQ(p1, p2);

  ++(*p1); CHECK_EQ(1, *p2);
}

struct Counter {
  static int cnt;

  Counter() { ++cnt; }
 ~Counter() { --cnt; }
};

int Counter::cnt = 0;

TEST_CASE("scoped<>") {
  {
    Counter *c;
    scoped<Counter*> p((c = new Counter()));
    CHECK_FALSE(p.isNull());
    CHECK(c == p.ptr());
    CHECK(c == static_cast<Counter*>(p));
    CHECK_EQ(1, p->cnt);
  }

  CHECK_EQ(0, Counter::cnt);

  owner<Counter*> raw;

  {
    scoped<Counter*> p(new Counter());
    CHECK_FALSE(p.isNull());
    CHECK_EQ(1, Counter::cnt);
    raw = p.take();
    CHECK(p.isNull());
    CHECK_EQ(1, Counter::cnt);
  }

  CHECK_EQ(1, Counter::cnt);

  {
    scoped<Counter*> p(raw);
    CHECK_FALSE(p.isNull());
    CHECK_EQ(1, Counter::cnt);
  }

  CHECK_EQ(0, Counter::cnt);
}

}
#endif
//------------------------------------------------------------------------------
// eof
