#include "gtest/gtest.h"
#include "core/typ/json.h"
#include "core/typ/realpair.h"


TEST(qpair, Initialization) {
    qpair qpair1;
    EXPECT_TRUE(qIsNaN(qpair1.x));
    EXPECT_TRUE(qIsNaN(qpair1.y));

    qpair qpair2(2.3, 3.4);
    EXPECT_EQ(2.3, qpair2.x);
    EXPECT_EQ(3.4, qpair2.y);
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
    qpair1.fromJson(qpair.toJson());
}
