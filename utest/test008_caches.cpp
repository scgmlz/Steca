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

// Minimal example to test and demonstrate usage of Cached<int>.
TEST(Caches, OneLevel) {
    static int N = 42;
    auto f = []()->int{ return {N++}; };
    lazy_data::Cached<int> cache{ f };
    EXPECT_EQ(42, cache.yield());
    EXPECT_EQ(42, cache.yield());
    cache.invalidate();
    EXPECT_EQ(43, cache.yield());
}
