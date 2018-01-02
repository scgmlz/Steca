#include "gtest/gtest.h"
#include "io/io_io.h"
#include "testdata.h"
#include "typ/exception.h"

TEST(IO, Caress) {
    try {
        io::load(TESTDATADIR "/caress.dat");
    } catch (Exception& ex) {
        std::cerr << "io::load throws: " << ex.what() << "\n";
        EXPECT_TRUE(false);
    } catch (...) {
        std::cerr << "io::load throws exception of unexpected type\n";
        EXPECT_TRUE(false);
    }
}
