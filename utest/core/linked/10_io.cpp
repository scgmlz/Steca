#include <iostream>
#include "core/loaders/loaders.h"
#include "core/session.h"
#include "core/base/exception.h"
#include "testdata.h"
#include "3rdparty/catch2/catch.hpp"

TEST_CASE( "IO - Caress", "" ) {
    try {
        load::loadRawfile(TESTDATADIR "/caress.dat");
    } catch (Exception& ex) {
        std::cerr << "io::load throws: " << ex.what() << "\n";
        CHECK(false);
    } catch (...) {
        std::cerr << "io::load throws exception of unexpected type\n";
        CHECK(false);
    }
    CHECK(true);
}
