#include "gtest/gtest.h"
#include "io/io_io.h"
#include "testdata.h"

TEST(IO, Caress) {
    try {
        io::loadCaress2(TESTDATADIR "/caress.dat");
    } catch (Exception& ex) {
        std::cerr << "io::loadCaress2 throws: " << ex.what() << "\n";
        EXPECT_TRUE(false);
    } catch (...) {
        std::cerr << "io::loadCaress2 throws exception of unexpected type\n";
        EXPECT_TRUE(false);
    }
}
