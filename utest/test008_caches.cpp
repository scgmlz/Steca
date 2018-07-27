#include "gtest/gtest.h"
#include "core/typ/cached.h"

class Payload {
public:
    Payload(double _x) : x(_x) {}
    double x;
};

static double xx = 17.32;

class Entry;

Payload recompute1(const Entry* const e)
{
    return xx++;
}

class Entry {
public:
    CachedElement<Entry, Payload, recompute1> cache {this};
private:
    friend Payload recompute1(const Entry* const e);
};

TEST(Caches, OneLevel) {
    Entry e;
    EXPECT_EQ(17.32, e.cache.get().x);
    EXPECT_EQ(17.32, e.cache.get().x);
    e.cache.invalidate();
    EXPECT_EQ(18.32, e.cache.get().x);
}
