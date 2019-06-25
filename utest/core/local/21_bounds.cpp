// test functions from unnamed namespace

// written in order to find out what lowerBound(..) and upperBound(..) are computing

#define CATCH_CONFIG_MAIN
#define LOCAL_CODE_ONLY
#include "core/data/angle_map.cpp"
#include "3rdparty/catch2/catch.hpp"

const double e=1e-15;

TEST_CASE( "Bounds - LowerBound", "[21_bounds]" ) {
    for (int n = 2; n<12; ++n) {
        std::vector<deg> v(n);
        for (int i=0; i<n; ++i)
            v[i] = i;
        CHECK(0 == lowerBound(v, Q_QNAN, 0, n));
        CHECK(0 == lowerBound(v, -1e99, 0, n));
        CHECK(0 == lowerBound(v, -e, 0, n));
        CHECK(0 == lowerBound(v, 0., 0, n));
        CHECK(1 == lowerBound(v, +e, 0, n));
        CHECK(1 == lowerBound(v, 1.-e, 0, n));
        CHECK(1 == lowerBound(v, 1., 0, n));
        CHECK(n-1 == lowerBound(v, n-2+e, 0, n));
        CHECK(n-1 == lowerBound(v, n-1-e, 0, n));
        CHECK(n-1 == lowerBound(v, n-1, 0, n));
        CHECK(n-1 == lowerBound(v, n-1+e, 0, n));
        CHECK(n-1 == lowerBound(v, n, 0, n));
        CHECK(n-1 == lowerBound(v, 1e99, 0, n));
    }
}

TEST_CASE( "Bounds - LowerBoundArg3", "[21_bounds]" ) {
    const int n = 8;
    std::vector<deg> v(n);
    for (int i=0; i<n; ++i)
        v[i] = i;
    CHECK(3 == lowerBound(v, Q_QNAN, 3, n));
    CHECK(3 == lowerBound(v, -1e99, 3, n));
    CHECK(3 == lowerBound(v, -e, 3, n));
    CHECK(3 == lowerBound(v, 0., 3, n));
    CHECK(3 == lowerBound(v, +e, 3, n));
    CHECK(3 == lowerBound(v, 1.-e, 3, n));
    CHECK(3 == lowerBound(v, 1., 3, n));
    CHECK(n-1 == lowerBound(v, n-2+e, 3, n));
    CHECK(n-1 == lowerBound(v, n-1-e, 3, n));
    CHECK(n-1 == lowerBound(v, n-1, 3, n));
    CHECK(n-1 == lowerBound(v, n-1+e, 3, n));
    CHECK(n-1 == lowerBound(v, n, 3, n));
    CHECK(n-1 == lowerBound(v, 1e99, 3, n));
}

TEST_CASE( "Bounds - LowerBoundArg4", "[21_bounds]" ) {
    const int n = 8;
    std::vector<deg> v(n);
    for (int i=0; i<n; ++i)
        v[i] = i;
    CHECK(0 == lowerBound(v, Q_QNAN, 0, 4));
    CHECK(0 == lowerBound(v, -1e99, 0, 4));
    CHECK(0 == lowerBound(v, -e, 0, 4));
    CHECK(0 == lowerBound(v, 0., 0, 4));
    CHECK(1 == lowerBound(v, +e, 0, 4));
    CHECK(1 == lowerBound(v, 1.-e, 0, 4));
    CHECK(1 == lowerBound(v, 1., 0, 4));
    CHECK(3 == lowerBound(v, n-2+e, 0, 4));
    CHECK(3 == lowerBound(v, n-1-e, 0, 4));
    CHECK(3 == lowerBound(v, n-1, 0, 4));
    CHECK(3 == lowerBound(v, n-1+e, 0, 4));
    CHECK(3 == lowerBound(v, n, 0, 4));
    CHECK(3 == lowerBound(v, 1e99, 0, 4));
}

TEST_CASE( "Bounds - UpperBound", "[21_bounds]" ) {
    for (int n = 2; n<12; ++n) {
        std::vector<deg> v(n);
        for (int i=0; i<n; ++i)
            v[i] = i;
        CHECK(1 == upperBound(v, -1e99, 0, n));
        CHECK(1 == upperBound(v, -e, 0, n));
        CHECK(1 == upperBound(v, 0., 0, n));
        CHECK(1 == upperBound(v, +e, 0, n));
        CHECK(1 == upperBound(v, 1.-e, 0, n));
        CHECK(2 == upperBound(v, 1., 0, n));
        CHECK(n-1 == upperBound(v, n-2, 0, n));
        CHECK(n-1 == upperBound(v, n-2+e, 0, n));
        CHECK(n-1 == upperBound(v, n-1-e, 0, n));
        CHECK(n == upperBound(v, n-1, 0, n));
        CHECK(n == upperBound(v, n-1+e, 0, n));
        CHECK(n == upperBound(v, n, 0, n));
        CHECK(n == upperBound(v, 1e99, 0, n));
        CHECK(n == upperBound(v, Q_QNAN, 0, n));
    }
}
