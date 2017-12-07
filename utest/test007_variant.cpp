#include "gtest/gtest.h"
#include "typ/typ_variant.h"

using typ::cmp_int;

TEST(Variant, Comparison) {
    QVariant v1(1), v2(2);
    EXPECT_EQ(0, cmp_int(v1, v1));
    EXPECT_EQ(-1, cmp_int(v1, v2));
    EXPECT_EQ(+1, cmp_int(v2, v1));
}
