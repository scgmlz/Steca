//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      utest/test008_caches.cpp
//! @brief     Tests and demonstrates usage of the data cache classes in namespace lazy_data.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gtest/gtest.h"
#include "core/typ/lazy_data.h"
#include <cmath>

// Minimal example to test and demonstrate usage of Cached.
TEST(Caches, Simple) {
    static int N = 42; // Auxiliary, to let the remake function depend on something.
    auto f = []()->int{ return N++; }; // The remake function. A complicated computation.
    lazy_data::Cached<int> cache{ f };
    EXPECT_EQ(42, cache.yield()); // recompute
    EXPECT_EQ(42, cache.yield()); // do not recompute
    cache.invalidate();
    EXPECT_EQ(43, cache.yield()); // recompute
}

// Test and demonstrate usage of Cached, with remake argument.
// Payload is double, argument is int.
TEST(Caches, SimpleWithArg) {
    auto f = [](int i)->double{ return sqrt(i); }; // The remake function.
    lazy_data::Cached<double,int> cache{ f };
    EXPECT_EQ(sqrt(2), cache.yield(2)); // recompute
    EXPECT_EQ(sqrt(2), cache.yield(5)); // do not recompute
    cache.invalidate();
    EXPECT_EQ(sqrt(5), cache.yield(5)); // recompute
}

// Minimal example to test and demonstrate usage of VectorCache.
TEST(Caches, Vector) {
    static int N = 10; // Auxiliary, to let the remake function depend on something.
    auto n = []()->int{ return 3; };           // The size function.
    auto f = [](int i)->int{ return N+i; }; // The remake function. A complicated computation.
    lazy_data::VectorCache<int> cache{ n, f };
    EXPECT_EQ(3, cache.size());
    EXPECT_EQ(10, cache.yield_at(0)); // recompute
    EXPECT_EQ(12, cache.yield_at(2)); // recompute
    N = 1000;
    EXPECT_EQ(10, cache.yield_at(0)); // do not recompute
    EXPECT_EQ(12, cache.yield_at(2)); // do not recompute
    cache.invalidate_at(2);
    EXPECT_EQ(10, cache.yield_at(0)); // do not recompute
    EXPECT_EQ(1002, cache.yield_at(2)); // recompute
    cache.clear_vector();
    EXPECT_EQ(1000, cache.yield_at(0)); // recompute
    EXPECT_EQ(1002, cache.yield_at(2)); // do not recompute
}
