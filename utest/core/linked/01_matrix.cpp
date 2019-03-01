#include "gtest/gtest.h"
#include "core/calc/matrix.h"

TEST(MatrixTest, BasicOperations) {
    mat3r m1(0, 1, 2, 3, 4, 5, 6, 7, 8);
    mat3r mt(0, 3, 6, 1, 4, 7, 2, 5, 8);
    mat3r mc(m1);

    EXPECT_EQ(m1.transposed(), mt);
    EXPECT_EQ(m1, mc);
    EXPECT_EQ(mt.transposed(), m1);

    EXPECT_EQ(vec3r(17, 62, 107), m1 * vec3r(4, 5, 6));
    EXPECT_EQ(mat3r(5, 14, 23, 14, 50, 86, 23, 86, 149), m1 * mt);
}

/*
TEST(MatrixTest, Rotation) {
    qreal angle = 1;
    auto cwx = mat3r::rotationCWx(angle);
    auto cwz = mat3r::rotationCWz(angle);
    auto ccwz = mat3r::rotationCCWz(angle);

    vec3r v(1, 2, 3);
    // TODO: test for _near_ equality
    EXPECT_EQ(v, cwx.transposed() * (cwx * v));
    EXPECT_EQ(v, ccwz * (cwz * v));
}
*/
