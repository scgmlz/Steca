#include "gtest/gtest.h"
#include "typ/typ_xy.h"
#include "typ/typ_json.h"

using typ::XY;

TEST(XY, Initialization) {
    XY xy1;
    EXPECT_TRUE(qIsNaN(xy1.x));
    EXPECT_TRUE(qIsNaN(xy1.y));

    XY xy2(2.3, 3.4);
    EXPECT_EQ(2.3, xy2.x);
    EXPECT_EQ(3.4, xy2.y);
}

TEST(XT, Comparisons) {
    XY xy(1, 2), xy1(1, 2), xy2(1, 0), xy3(2, 2);
    EXPECT_EQ(0, xy.compare(xy));
    EXPECT_EQ(0, xy.compare(xy1));
    EXPECT_EQ(1, xy.compare(xy2));
    EXPECT_EQ(-1, xy.compare(xy3));

    EXPECT_EQ(xy, xy1);
    EXPECT_NE(xy, xy2);
}

TEST(XY, Validity) {
    XY xy;
    EXPECT_TRUE(!xy.isValid());
    xy.x = 0;
    EXPECT_TRUE(!xy.isValid());
    xy.y = 0;
    EXPECT_TRUE(xy.isValid());
    xy.invalidate();
    EXPECT_TRUE(!xy.isValid());
}

TEST(XY, Json) {
    XY xy(-1, 2), xy1;
    xy1.loadJson(xy.saveJson());
    EXPECT_EQ(xy, xy1);
}
