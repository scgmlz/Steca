#include "core/typ/json.h"
#include "core/typ/range.h"
#include <vector>
#include "3rdparty/catch2/catch.hpp"

static bool RANGE_EQ(Range const& r1, Range const& r2) {
    return r1.min == r2.min && r1.max == r2.max;
}

TEST_CASE( "Range - Trivia", "" ) {
    Range r0;
    CHECK(qIsNaN(r0.min));
    CHECK(qIsNaN(r0.max));
    Range r1(1, 2), r2(r1);
    CHECK(RANGE_EQ(r1, r2));
    Range r3(6,6);
    CHECK(6 == r3.min);
    CHECK(6 == r3.max);
    Range r4(6, 7);
    CHECK(6 == r4.min);
    CHECK(7 == r4.max);
}

TEST_CASE( "Range - Validity", "" ) {
    Range r;
    CHECK(!r.isValid());
    r.min = 0;
    CHECK(!r.isValid());
    r.max = 0;
    CHECK(r.isValid());
    r.invalidate();
    CHECK(!r.isValid());
    r.set(0,0);
    CHECK(r.isValid());
}

TEST_CASE( "Range - IsEmpty", "" ) {
    CHECK(Range().isEmpty());

    Range r(0,0);
    CHECK(r.isEmpty());
    r.max = 1;
    CHECK(!r.isEmpty());
    r.min = 2;
    CHECK(r.isEmpty());
    r.max = Q_QNAN;
    CHECK(r.isEmpty());
}

TEST_CASE( "Range - Width", "" ) {
    CHECK(qIsNaN(Range().width()));
    CHECK(0 == Range(0,0).width());
    CHECK(qIsInf(Range(0, Q_INFINITY).width()));

    Range r(0,0);
    CHECK(r.isEmpty());
    r.max = 1;
    CHECK(!r.isEmpty());
    r.min = 2;
    CHECK(r.isEmpty());
    r.max = Q_QNAN;
    CHECK(r.isEmpty());
}

TEST_CASE( "Range - Center", "" ) {
    CHECK(qIsNaN(Range().center()));
    CHECK(0 == Range(0,0).center());
    CHECK(qIsNaN(Range(0, Q_QNAN).center()));
    CHECK(qIsInf(Range(0, Q_INFINITY).center()));
}

TEST_CASE( "Range - Safe", "" ) {
    auto r = Range::safeFrom(2, 3);
    CHECK(RANGE_EQ(r, Range(2, 3)));
    r = Range::safeFrom(3, 2);
    CHECK(RANGE_EQ(r, Range(2, 3)));
    r = Range::safeFrom(3, 4);
    CHECK(RANGE_EQ(r, Range(3, 4)));
    r = Range::safeFrom(4, 3);
    CHECK(RANGE_EQ(r, Range(3, 4)));
}

TEST_CASE( "Range - Extend", "" ) {
    auto r = Range(1, 2);
    r.extendBy(-1);
    CHECK(RANGE_EQ(r, Range(-1, 2)));
    r.extendBy(Range(3, 4));
    CHECK(RANGE_EQ(r, Range(-1, 4)));
}

TEST_CASE( "Range - Contains", "" ) {
    auto r = Range(-1, +1);

    CHECK(!Range().contains(r));
    CHECK(!Range().contains(Range()));

    CHECK(!r.contains(Range()));
    CHECK(!r.contains(Q_QNAN));
    CHECK(!r.contains(Q_INFINITY));

    CHECK(r.contains(r));

    CHECK(r.contains(-1));
    CHECK(r.contains(0));
    CHECK(r.contains(+1));
    CHECK(r.contains(Range(0, 1)));
    CHECK(!r.contains(Range(-2, 0)));
}

TEST_CASE( "Range - Intersects", "" ) {
    auto r = Range(-1, +1);

    CHECK(!Range().intersects(r));
    CHECK(!Range().intersects(Range()));

    CHECK(!r.intersects(Range()));
    CHECK(r.intersects(r));

    CHECK(r.intersects(Range(0, 10)));
    CHECK(r.intersects(Range(-2, 0)));
}

TEST_CASE( "Range - Intersect", "" ) {
    auto r = Range(-1, +1);

    CHECK(!Range().intersect(r).isValid());
    CHECK(!r.intersect(Range()).isValid());

    CHECK(!r.intersect(Range()).isValid());

    auto disjoint = Range(-3, -2);
    CHECK(r.intersect(disjoint).isEmpty());
}

typedef struct { qreal min, max; } min_max;

/* TODO restore to/fromJson

static bool RANGES_EQ(Ranges const& rs1, Ranges const& rs2) {
    if (rs1.count() != rs2.count())
        return false;

    for (int i=0; i<rs1.count(); ++i) {
        if (! RANGE_EQ(rs1.at(i), rs2.at(i)) )
            return false;
    }

    return true;
}

TEST(Ranges, Json) {
    Ranges rs, rs1;
    rs.add(Range());
    rs.add(Range(9,9));
    rs.add(Range(-3, -2));
    rs1.fromJson(rs.toJson());
    EXPECT_TRUE(RANGES_EQ(rs, rs1));
}

*/
