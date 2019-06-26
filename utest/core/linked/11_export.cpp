// test functions from unnamed namespaces in gui/output

#include "core/calc/export.h"
#include "3rdparty/catch2/catch.hpp"

TEST_CASE( "AnonymousOutput - NumberedFileName", "" ) {
    using namespace data_export;
    //ASSERT_DEATH(numberedFileName("bla",1,1), "");
    CHECK("1" == numberedFileName("%d",1,1));
    CHECK("1" == numberedFileName("%d",1,9));
    CHECK("9" == numberedFileName("%d",9,9));
    CHECK("01" == numberedFileName("%d",1,10));
    CHECK("10" == numberedFileName("%d",10,10));
    CHECK("01" == numberedFileName("%d",1,99));
    CHECK("99" == numberedFileName("%d",99,99));
    CHECK("001" == numberedFileName("%d",1,100));
    CHECK("010" == numberedFileName("%d",10,100));
    CHECK("100" == numberedFileName("%d",100,100));
}
