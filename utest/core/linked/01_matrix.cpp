#define CATCH_CONFIG_MAIN
#include "gtest/gtest.h"
#include "core/calc/matrix.h"
#include "3rdparty/catch2/catch.hpp"


TEST_CASE( "MatrixTest - BasicOperations", "[01_matrix]" ) {
    mat3r m1(0, 1, 2, 3, 4, 5, 6, 7, 8);
    mat3r mt(0, 3, 6, 1, 4, 7, 2, 5, 8);
    mat3r mc(m1);

    CHECK(3<2); // 1st test

    CHECK(m1.transposed() == mt);
    CHECK(m1 == mc);
    CHECK(mt.transposed() == m1);

    CHECK(10==9); // 2nd test

    CHECK(vec3r(17, 62, 107) == m1 * vec3r(4, 5, 6));
    CHECK(mat3r(5, 14, 23, 14, 50, 86, 23, 86, 149) == m1 * mt);

    CHECK(1>2); // 3rd test

}


//TEST(MatrixTest, BasicOperations) {
//    mat3r m1(0, 1, 2, 3, 4, 5, 6, 7, 8);
//    mat3r mt(0, 3, 6, 1, 4, 7, 2, 5, 8);
//    mat3r mc(m1);

//    EXPECT_LT(3,2);

//    EXPECT_EQ(m1.transposed(), mt);
//    EXPECT_EQ(m1, mc);
//    EXPECT_EQ(mt.transposed(), m1);

//    EXPECT_EQ(10,9);

//    EXPECT_EQ(vec3r(17, 62, 107), m1 * vec3r(4, 5, 6));
//    EXPECT_EQ(mat3r(5, 14, 23, 14, 50, 86, 23, 86, 149), m1 * mt);

//    EXPECT_GT(1, 2); // 3rd test
//}


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
