// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/def/def_gsl.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "def_gsl.h"
#include "wrap_doctest.h"

qreal const NAN = Q_QNAN;
qreal const INF = Q_INFINITY;

TEST("NAN/INF", ({
         double d;
         d = float(NAN);
         CHECK(qIsNaN(d));
         d = float(INF);
         CHECK(qIsInf(d));
         float f;
         f = float(NAN);
         CHECK(qIsNaN(f));
         f = float(INF);
         CHECK(qIsInf(f));
     });)

#ifdef TESTS
namespace {

TEST_CASE("not_null<>") {
    int i = 0;
    auto p1 = not_null<int*>::from(&i), p2(p1);
    CHECK_EQ(p1, p2);

    ++(*p1);
    CHECK_EQ(1, *p2);
}

struct Counter {
    static int cnt;

    Counter() { ++cnt; }
    ~Counter() { --cnt; }
};

int Counter::cnt = 0;

TEST_CASE("scoped<>") {
    {
        Counter* c;
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
