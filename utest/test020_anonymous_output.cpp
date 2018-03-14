// test functions from unnamed namespaces in gui/output

#include "gtest/gtest.h"
#include "gui/dialogs/output_diffractograms.cpp"

TEST(AnonymousOutput, NumberedName) {
    ASSERT_DEATH(numberedName("bla",1,1), "");
    EXPECT_EQ("1", numberedName("%d",1,1));
    EXPECT_EQ("1", numberedName("%d",1,9));
    EXPECT_EQ("9", numberedName("%d",9,9));
    EXPECT_EQ("01", numberedName("%d",1,10));
    EXPECT_EQ("10", numberedName("%d",10,10));
    EXPECT_EQ("01", numberedName("%d",1,99));
    EXPECT_EQ("99", numberedName("%d",99,99));
    EXPECT_EQ("001", numberedName("%d",1,100));
    EXPECT_EQ("010", numberedName("%d",10,100));
    EXPECT_EQ("100", numberedName("%d",100,100));
}
