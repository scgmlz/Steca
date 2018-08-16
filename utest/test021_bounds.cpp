// test functions from unnamed namespace

// written in order to find out what lowerBound(..) and upperBound(..) are computing

#include "gtest/gtest.h"
#include "core/data/angle_map.cpp"

const double e=1e-15;

TEST(Bounds, LowerBound) {
    for (int n = 2; n<12; ++n) {
        std::vector<deg> v(n);
        for (int i=0; i<n; ++i)
            v[i] = i;
        EXPECT_EQ(0, lowerBound(v, Q_QNAN, 0, n));
        EXPECT_EQ(0, lowerBound(v, -1e99, 0, n));
        EXPECT_EQ(0, lowerBound(v, -e, 0, n));
        EXPECT_EQ(0, lowerBound(v, 0., 0, n));
        EXPECT_EQ(1, lowerBound(v, +e, 0, n));
        EXPECT_EQ(1, lowerBound(v, 1.-e, 0, n));
        EXPECT_EQ(1, lowerBound(v, 1., 0, n));
        EXPECT_EQ(n-1, lowerBound(v, n-2+e, 0, n));
        EXPECT_EQ(n-1, lowerBound(v, n-1-e, 0, n));
        EXPECT_EQ(n-1, lowerBound(v, n-1, 0, n));
        EXPECT_EQ(n-1, lowerBound(v, n-1+e, 0, n));
        EXPECT_EQ(n-1, lowerBound(v, n, 0, n));
        EXPECT_EQ(n-1, lowerBound(v, 1e99, 0, n));
    }
}

TEST(Bounds, LowerBoundArg3) {
    const int n = 8;
    std::vector<deg> v(n);
    for (int i=0; i<n; ++i)
        v[i] = i;
    EXPECT_EQ(3, lowerBound(v, Q_QNAN, 3, n));
    EXPECT_EQ(3, lowerBound(v, -1e99, 3, n));
    EXPECT_EQ(3, lowerBound(v, -e, 3, n));
    EXPECT_EQ(3, lowerBound(v, 0., 3, n));
    EXPECT_EQ(3, lowerBound(v, +e, 3, n));
    EXPECT_EQ(3, lowerBound(v, 1.-e, 3, n));
    EXPECT_EQ(3, lowerBound(v, 1., 3, n));
    EXPECT_EQ(n-1, lowerBound(v, n-2+e, 3, n));
    EXPECT_EQ(n-1, lowerBound(v, n-1-e, 3, n));
    EXPECT_EQ(n-1, lowerBound(v, n-1, 3, n));
    EXPECT_EQ(n-1, lowerBound(v, n-1+e, 3, n));
    EXPECT_EQ(n-1, lowerBound(v, n, 3, n));
    EXPECT_EQ(n-1, lowerBound(v, 1e99, 3, n));
}

TEST(Bounds, LowerBoundArg4) {
    const int n = 8;
    std::vector<deg> v(n);
    for (int i=0; i<n; ++i)
        v[i] = i;
    EXPECT_EQ(0, lowerBound(v, Q_QNAN, 0, 4));
    EXPECT_EQ(0, lowerBound(v, -1e99, 0, 4));
    EXPECT_EQ(0, lowerBound(v, -e, 0, 4));
    EXPECT_EQ(0, lowerBound(v, 0., 0, 4));
    EXPECT_EQ(1, lowerBound(v, +e, 0, 4));
    EXPECT_EQ(1, lowerBound(v, 1.-e, 0, 4));
    EXPECT_EQ(1, lowerBound(v, 1., 0, 4));
    EXPECT_EQ(3, lowerBound(v, n-2+e, 0, 4));
    EXPECT_EQ(3, lowerBound(v, n-1-e, 0, 4));
    EXPECT_EQ(3, lowerBound(v, n-1, 0, 4));
    EXPECT_EQ(3, lowerBound(v, n-1+e, 0, 4));
    EXPECT_EQ(3, lowerBound(v, n, 0, 4));
    EXPECT_EQ(3, lowerBound(v, 1e99, 0, 4));
}

TEST(Bounds, UpperBound) {
    for (int n = 2; n<12; ++n) {
        std::vector<deg> v(n);
        for (int i=0; i<n; ++i)
            v[i] = i;
        EXPECT_EQ(1, upperBound(v, -1e99, 0, n));
        EXPECT_EQ(1, upperBound(v, -e, 0, n));
        EXPECT_EQ(1, upperBound(v, 0., 0, n));
        EXPECT_EQ(1, upperBound(v, +e, 0, n));
        EXPECT_EQ(1, upperBound(v, 1.-e, 0, n));
        EXPECT_EQ(2, upperBound(v, 1., 0, n));
        EXPECT_EQ(n-1, upperBound(v, n-2, 0, n));
        EXPECT_EQ(n-1, upperBound(v, n-2+e, 0, n));
        EXPECT_EQ(n-1, upperBound(v, n-1-e, 0, n));
        EXPECT_EQ(n, upperBound(v, n-1, 0, n));
        EXPECT_EQ(n, upperBound(v, n-1+e, 0, n));
        EXPECT_EQ(n, upperBound(v, n, 0, n));
        EXPECT_EQ(n, upperBound(v, 1e99, 0, n));
        EXPECT_EQ(n, upperBound(v, Q_QNAN, 0, n));
    }
}
