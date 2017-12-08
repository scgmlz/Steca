#include "def/exception.h"
#include "def/scoped.h"
#include "gtest/gtest.h"

TEST(Exception, Basics) {
    EXPECT_THROW(THROW("Hello"), Exception);
    EXPECT_THROW(THROW_SILENT(), Exception);
    EXPECT_NO_THROW(Exception());
    EXPECT_THROW(Exception().raise(), Exception);
    scoped<Exception*> p(Exception().clone());
    EXPECT_THROW(p->raise(), Exception);
}

TEST(Exception, Msg) {
    try { THROW("Hi"); }
    catch (Exception& e) {
        EXPECT_EQ("Hi", e.msg());
        return;
    }
    EXPECT_TRUE(false); // not here
}

TEST(Exception, What) {
    try { THROW("Hi"); }
    catch (Exception& e) {
        EXPECT_EQ("Hi", str(e.what()));
        return;
    }
    EXPECT_TRUE(false); // not here
}
