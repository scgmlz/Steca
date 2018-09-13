#include "gtest/gtest.h"
#include "core/misc/angles.h"
#include <qmath.h>

TEST(Angles, Conversion) {
    EXPECT_EQ(qreal(deg(rad(M_PI_2))), 90);
    EXPECT_EQ(qreal(rad(deg(90))), M_PI_2);
}

TEST(Angles, Normalization) {
    EXPECT_EQ(qreal(deg(10)),qreal(deg(730).normalized()));
    EXPECT_EQ(qreal(deg(350)),qreal(deg(-730).normalized()));
}
