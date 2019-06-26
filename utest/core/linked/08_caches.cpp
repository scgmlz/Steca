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

#include "core/typ/lazy_data.h"
#include <cmath>
#include "3rdparty/catch2/catch.hpp"

// Minimal example to test and demonstrate usage of Cached.
TEST_CASE( "Caches - Simple", "" ) {
    static int N = 42; // Auxiliary, to let the remake function depend on something.
    auto f = []()->int{ return N++; }; // The remake function. A complicated computation.
    lazy_data::Cached<int> cache{ f };
    CHECK(42 == cache.yield()); // recompute
    CHECK(42 == cache.yield()); // do not recompute
    cache.invalidate();
    CHECK(43 == cache.yield()); // recompute
}

// Test and demonstrate usage of Cached, with remake argument.
// Payload is double, argument is int.
TEST_CASE( "Caches - SimpleWithArg", "" ) {
    auto f = [](int i)->double{ return sqrt(i); }; // The remake function.
    lazy_data::Cached<double,int> cache{ f };
    CHECK(sqrt(2) == cache.yield(2)); // recompute
    CHECK(sqrt(2) == cache.yield(5)); // do not recompute
    cache.invalidate();
    CHECK(sqrt(5) == cache.yield(5)); // recompute
}

// Minimal example to test and demonstrate usage of VectorCache.
TEST_CASE( "Caches - Vector", "" ) {
    static int N = 10; // Auxiliary, to let the remake function depend on something.
    auto n = []()->int{ return 3; };           // The size function.
    auto f = [](int i)->int{ return N+i; }; // The remake function. A complicated computation.
    lazy_data::VectorCache<int> cache{ n, f };
    CHECK(3 == cache.size());
    CHECK(10 == cache.yield_at(0)); // recompute
    CHECK(12 == cache.yield_at(2)); // recompute
    N = 1000;
    CHECK(10 == cache.yield_at(0)); // do not recompute
    CHECK(12 == cache.yield_at(2)); // do not recompute
    cache.invalidate_at(2);
    CHECK(10 == cache.yield_at(0)); // do not recompute
    CHECK(1002 == cache.yield_at(2)); // recompute
    cache.clear_vector();
    CHECK(1000 == cache.yield_at(0)); // recompute
    CHECK(1002 == cache.yield_at(2)); // do not recompute
}
