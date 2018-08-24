#include "gtest/gtest.h"
#include "core/loaders/loaders.h"
#include "core/session.h"
#include "core/aux/exception.h"
#include "testdata.h"

TEST(IO, Caress) {
    try {
        load::loadRawfile(TESTDATADIR "/caress.dat");
    } catch (Exception& ex) {
        std::cerr << "io::load throws: " << ex.what() << "\n";
        EXPECT_TRUE(false);
    } catch (...) {
        std::cerr << "io::load throws exception of unexpected type\n";
        EXPECT_TRUE(false);
    }
}
