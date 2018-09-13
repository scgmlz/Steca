// test functions from unnamed namespaces in gui/output

#include "gtest/gtest.h"
#include "gui/dialogs/export_dfgram.cpp"

TEST(AnonymousOutput, NumberedFileName) {
    ASSERT_DEATH(numberedFileName("bla",1,1), "");
    EXPECT_EQ("1", numberedFileName("%d",1,1));
    EXPECT_EQ("1", numberedFileName("%d",1,9));
    EXPECT_EQ("9", numberedFileName("%d",9,9));
    EXPECT_EQ("01", numberedFileName("%d",1,10));
    EXPECT_EQ("10", numberedFileName("%d",10,10));
    EXPECT_EQ("01", numberedFileName("%d",1,99));
    EXPECT_EQ("99", numberedFileName("%d",99,99));
    EXPECT_EQ("001", numberedFileName("%d",1,100));
    EXPECT_EQ("010", numberedFileName("%d",10,100));
    EXPECT_EQ("100", numberedFileName("%d",100,100));
}
