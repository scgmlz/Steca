#include "gtest/gtest.h"
#include "lazy_data.h"

class Payload {
public:
    Payload(int _x) : x(_x) {}
    int x;
};

static int xx;

class Entry {
public:
    lazy_data::Cached<Payload> cache{ []()->Payload{ return {xx++}; } };
};

TEST(Caches, OneLevel) {
    xx = 5;

    Entry e;
    EXPECT_EQ(5, e.cache.yield().x);
    EXPECT_EQ(5, e.cache.yield().x);
    e.cache.invalidate();
    EXPECT_EQ(6, e.cache.yield().x);

    Entry e2;
    EXPECT_EQ(7, e2.cache.yield().x);
    e2.cache.invalidate();
    EXPECT_EQ(8, e2.cache.yield().x);

    EXPECT_EQ(6, e.cache.yield().x);
}
