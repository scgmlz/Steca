#include "gtest/gtest.h"
#include "typ/json.h"
#include "typ/qpair.h"


TEST(qpair, Initialization) {
    qpair qpair1;
    EXPECT_TRUE(qIsNaN(qpair1.x));
    EXPECT_TRUE(qIsNaN(qpair1.y));

    qpair qpair2(2.3, 3.4);
    EXPECT_EQ(2.3, qpair2.x);
    EXPECT_EQ(3.4, qpair2.y);
}

TEST(XT, Comparisons) {
    qpair qpair(1, 2), qpair1(1, 2), qpair2(1, 0), qpair3(2, 2);
    EXPECT_EQ(0, qpair.compare(qpair));
    EXPECT_EQ(0, qpair.compare(qpair1));
    EXPECT_EQ(1, qpair.compare(qpair2));
    EXPECT_EQ(-1, qpair.compare(qpair3));

    EXPECT_EQ(qpair, qpair1);
    EXPECT_NE(qpair, qpair2);
}

TEST(qpair, Validity) {
    qpair qpair;
    EXPECT_TRUE(!qpair.isValid());
    qpair.x = 0;
    EXPECT_TRUE(!qpair.isValid());
    qpair.y = 0;
    EXPECT_TRUE(qpair.isValid());
    qpair.invalidate();
    EXPECT_TRUE(!qpair.isValid());
}

TEST(qpair, Json) {
    qpair qpair(-1, 2), qpair1;
    qpair1.loadJson(qpair.saveJson());
    EXPECT_EQ(qpair, qpair1);
}
