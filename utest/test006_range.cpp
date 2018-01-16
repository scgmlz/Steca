#include "gtest/gtest.h"
#include "core/def/idiomatic_for.h"
#include "core/typ/json.h"
#include "core/typ/range.h"
#include <vector>

static bool RANGE_EQ(Range const& r1, Range const& r2) {
    return r1.min == r2.min && r1.max == r2.max;
}

TEST(Range, Trivia) {
    Range r0;
    EXPECT_TRUE(qIsNaN(r0.min));
    EXPECT_TRUE(qIsNaN(r0.max));
    Range r1(1, 2), r2(r1);
    EXPECT_TRUE(RANGE_EQ(r1, r2));
    Range r3(6,6);
    EXPECT_EQ(6, r3.min);
    EXPECT_EQ(6, r3.max);
    Range r4(6, 7);
    EXPECT_EQ(6, r4.min);
    EXPECT_EQ(7, r4.max);
    auto r5 = Range::infinite();
    EXPECT_TRUE(qIsInf(r5.min));
    EXPECT_LE(r5.min, 0);
    EXPECT_TRUE(qIsInf(r5.max));
    EXPECT_GE(r5.max, 0);
}

TEST(Range, Validity) {
    Range r;
    EXPECT_TRUE(!r.isValid());
    r.min = 0;
    EXPECT_TRUE(!r.isValid());
    r.max = 0;
    EXPECT_TRUE(r.isValid());
    r.invalidate();
    EXPECT_TRUE(!r.isValid());
    r.set(0,0);
    EXPECT_TRUE(r.isValid());
}

TEST(Range, IsEmpty) {
    EXPECT_TRUE(Range().isEmpty());
    EXPECT_TRUE(!Range::infinite().isEmpty());

    Range r(0,0);
    EXPECT_TRUE(r.isEmpty());
    r.max = 1;
    EXPECT_TRUE(!r.isEmpty());
    r.min = 2;
    EXPECT_TRUE(r.isEmpty());
    r.max = NAN;
    EXPECT_TRUE(r.isEmpty());
}

TEST(Range, Width) {
    EXPECT_TRUE(qIsNaN(Range().width()));
    EXPECT_EQ(0, Range(0,0).width());
    EXPECT_TRUE(qIsInf(Range(0, INF).width()));
    EXPECT_TRUE(qIsInf(Range::infinite().width()));

    Range r(0,0);
    EXPECT_TRUE(r.isEmpty());
    r.max = 1;
    EXPECT_TRUE(!r.isEmpty());
    r.min = 2;
    EXPECT_TRUE(r.isEmpty());
    r.max = NAN;
    EXPECT_TRUE(r.isEmpty());
}

TEST(Range, Center) {
    EXPECT_TRUE(qIsNaN(Range().center()));
    EXPECT_EQ(0, Range(0,0).center());
    EXPECT_TRUE(qIsNaN(Range(0, NAN).center()));
    EXPECT_TRUE(qIsInf(Range(0, INF).center()));
    EXPECT_TRUE(qIsNaN(Range::infinite().center()));
}

TEST(Range, Safe) {
    auto r = Range::safeFrom(2, 3);
    RANGE_EQ(r, Range(2, 3));
    r = Range::safeFrom(3, 2);
    RANGE_EQ(r, Range(2, 3));
    r.safeSet(3, 4);
    RANGE_EQ(r, Range(3, 4));
    r.safeSet(4, 3);
    RANGE_EQ(r, Range(3, 4));
    r.safeSet(+INF, -INF);
    RANGE_EQ(r, Range::infinite());
}

TEST(Range, Extend) {
    auto r = Range(1, 2);
    r.extendBy(-1);
    RANGE_EQ(r, Range(-1, 2));
    r.extendBy(Range(3, 4));
    RANGE_EQ(r, Range(-1, 4));
}

TEST(Range, Contains) {
    auto r = Range(-1, +1);

    EXPECT_TRUE(!Range().contains(r));
    EXPECT_TRUE(!Range().contains(Range()));
    EXPECT_TRUE(!Range().contains(Range::infinite()));

    EXPECT_TRUE(Range::infinite().contains(r));
    EXPECT_TRUE(Range::infinite().contains(Range::infinite()));
    EXPECT_TRUE(!Range::infinite().contains(Range()));

    EXPECT_TRUE(!r.contains(Range()));
    EXPECT_TRUE(!r.contains(Range::infinite()));
    EXPECT_TRUE(!r.contains(NAN));
    EXPECT_TRUE(!r.contains(INF));

    EXPECT_TRUE(r.contains(r));

    EXPECT_TRUE(r.contains(-1));
    EXPECT_TRUE(r.contains(0));
    EXPECT_TRUE(r.contains(+1));
    EXPECT_TRUE(r.contains(Range(0, 1)));
    EXPECT_TRUE(!r.contains(Range(-2, 0)));
}

TEST(Range, Intersects) {
    auto r = Range(-1, +1);

    EXPECT_TRUE(!Range().intersects(r));
    EXPECT_TRUE(!Range().intersects(Range()));
    EXPECT_TRUE(!Range().intersects(Range::infinite()));

    EXPECT_TRUE(Range::infinite().intersects(r));
    EXPECT_TRUE(Range::infinite().intersects(Range::infinite()));
    EXPECT_TRUE(!Range::infinite().intersects(Range()));

    EXPECT_TRUE(!r.intersects(Range()));
    EXPECT_TRUE(r.intersects(Range::infinite()));

    EXPECT_TRUE(r.intersects(r));

    EXPECT_TRUE(r.intersects(Range(0, 10)));
    EXPECT_TRUE(r.intersects(Range(-2, 0)));
}

TEST(Range, Intersect) {
    auto r = Range(-1, +1);

    EXPECT_TRUE(!Range().intersect(r).isValid());
    EXPECT_TRUE(!r.intersect(Range()).isValid());
    EXPECT_TRUE(!Range().intersect(Range::infinite()).isValid());

    EXPECT_EQ(r, Range::infinite().intersect(r));
    EXPECT_EQ(Range::infinite(), Range::infinite().intersect(Range::infinite()));
    EXPECT_TRUE(!Range::infinite().intersect(Range()).isValid());

    EXPECT_TRUE(!r.intersect(Range()).isValid());
    EXPECT_EQ(r, r.intersect(Range::infinite()));

    EXPECT_EQ(r, r.intersect(r));

    EXPECT_EQ(Range(0, 1), r.intersect(Range(0, 10)));
    EXPECT_EQ(Range(-1, 0), r.intersect(Range(-2, 0)));

    auto disjoint = Range(-3, -2);
    EXPECT_TRUE(r.intersect(disjoint).isEmpty());
    EXPECT_EQ(r.min, r.intersect(disjoint).min);
}

TEST(Range, Bound) {
    auto r = Range(-1, +1);

    EXPECT_TRUE(qIsNaN(Range().bound(0)));
    EXPECT_TRUE(qIsNaN(Range().bound(INF)));
    EXPECT_TRUE(qIsNaN(Range().bound(NAN)));
    EXPECT_EQ(0, Range::infinite().bound(0));
    EXPECT_TRUE(qIsInf(Range::infinite().bound(INF)));
    EXPECT_TRUE(qIsNaN(Range::infinite().bound(NAN)));

    EXPECT_EQ(0, r.bound(0));
    EXPECT_EQ(-1, r.bound(-10));
    EXPECT_EQ(-1, r.bound(-INF));
    EXPECT_EQ(+1, r.bound(+10));
    EXPECT_EQ(+1, r.bound(+INF));
}

TEST(Range, Json) {
    Range r(-1, 2), r1;
    r1.from_json(r.to_json());
    EXPECT_EQ(r, r1);
}

typedef struct { qreal min, max; } min_max;

static bool RANGES_EQ(Ranges const& rs, std::vector<min_max> mm) {
    if (rs.count() != (int)mm.size())
        return false;

    for_i (rs.count()) {
        auto r = rs.at(i);
        auto m = mm.at(i);
        if (r.min != m.min || r.max != m.max)
            return false;
    }

    return true;
}

static bool RANGES_EQ(Ranges const& rs1, Ranges const& rs2) {
    if (rs1.count() != rs2.count())
        return false;

    for_i (rs1.count()) {
        if (rs1.at(i) != rs2.at(i))
            return false;
    }

    return true;
}

TEST(Ranges, Trivia) {
    Ranges rs;
    EXPECT_TRUE(rs.isEmpty());
    EXPECT_TRUE(RANGES_EQ(rs, rs));
    EXPECT_TRUE(RANGES_EQ(rs, Ranges()));

    Range r1(0, 1), r2(1, 2), r3(2, 3), r4(3, 4);

    EXPECT_TRUE(rs.add(r4));
    EXPECT_FALSE(rs.add(r4));
    EXPECT_TRUE(RANGES_EQ(rs, { { 3, 4 } }));

    EXPECT_TRUE(rs.add(r1));
    EXPECT_FALSE(rs.add(r1));
    EXPECT_TRUE(RANGES_EQ(rs, { { 0, 1 }, { 3, 4 } }));

    EXPECT_TRUE(rs.add(r2));
    EXPECT_TRUE(RANGES_EQ(rs, { { 0, 2 }, { 3, 4 } }));

    EXPECT_TRUE(rs.add(r3));
    EXPECT_TRUE(RANGES_EQ(rs, { { 0, 4 } }));

    EXPECT_TRUE(rs.remove(r2));
    EXPECT_FALSE(rs.remove(r2));
    EXPECT_TRUE(RANGES_EQ(rs, { { 0, 1 }, { 2, 4 } }));

    rs.clear();
    EXPECT_FALSE(rs.remove(r1));
}

TEST(Ranges, Json) {
    Ranges rs, rs1;
    rs.add(Range());
    rs.add(Range(9,9));
    rs.add(Range(-3, -2));
    rs.add(Range::infinite());

    rs1.from_json(rs.to_json());
    EXPECT_TRUE(RANGES_EQ(rs, rs1));
}
