#include "gtest/gtest.h"
#include "def/def_gsl.h"
#include "def/def_gsl.h" // declares 'scoped'

TEST(GSL, NanInf) {
    double d;
    d = float(NAN);
    EXPECT_TRUE(qIsNaN(d));
    d = float(INF);
    EXPECT_TRUE(qIsInf(d));
    float f;
    f = float(NAN);
    EXPECT_TRUE(qIsNaN(f));
    f = float(INF);
    EXPECT_TRUE(qIsInf(f));
}

TEST(GSL, NotNull) {
    int i = 0;
    auto p1 = not_null<int*>::from(&i), p2(p1);
    EXPECT_EQ(p1, p2);

    ++(*p1);
    EXPECT_EQ(1, *p2);
}

struct Counter {
    static int cnt;

    Counter() { ++cnt; }
    ~Counter() { --cnt; }
};

int Counter::cnt = 0;

TEST(GSL, Scoped) {
    {
        Counter* c;
        scoped<Counter*> p((c = new Counter()));
        EXPECT_FALSE(p.isNull());
        EXPECT_TRUE(c == p.ptr());
        EXPECT_TRUE(c == static_cast<Counter*>(p));
        EXPECT_EQ(1, p->cnt);
    }
    EXPECT_EQ(0, Counter::cnt);

    owner<Counter*> raw;

    {
        scoped<Counter*> p(new Counter());
        EXPECT_FALSE(p.isNull());
        EXPECT_EQ(1, Counter::cnt);
        raw = p.take();
        EXPECT_TRUE(p.isNull());
        EXPECT_EQ(1, Counter::cnt);
    }

    EXPECT_EQ(1, Counter::cnt);

    {
        scoped<Counter*> p(raw);
        EXPECT_FALSE(p.isNull());
        EXPECT_EQ(1, Counter::cnt);
    }

    EXPECT_EQ(0, Counter::cnt);
}
