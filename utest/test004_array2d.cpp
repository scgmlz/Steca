#include "gtest/gtest.h"
#include "typ/array2d.h"

using typ::size2d;

TEST(Array2d, Size2d) {
    size2d sz;
    EXPECT_TRUE((0 == sz.w && 0 == sz.h && sz.isEmpty() && 0 == sz.count()));

    size2d sz1(1, 2);
    EXPECT_TRUE((1 == sz1.w && 2 == sz1.h && !sz1.isEmpty() && 2 == sz1.count()));

    sz = sz1.transposed();
    EXPECT_TRUE((sz.w == sz1.h && sz.h == sz1.w));
}
