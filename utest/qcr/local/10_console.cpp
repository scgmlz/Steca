#include "gtest/gtest.h"
#define LOCAL_CODE_ONLY
#include "qcr/engine/console.cpp"

void testPCL(const QString& input, const QString& expectedCommand, const QString& expectedContext)
{
    QString command, context;
    bool ret = parseCommandLine(input, command, context);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(command, expectedCommand);
    EXPECT_EQ(context, expectedContext);
}

TEST(Console, parseCommandLine) {
    testPCL("quit", "quit", "");
    testPCL("  quit  ", "quit", "");
    testPCL("[    0ms main log] # Steca 2.1.2 started ...", "", "main");
    testPCL("[   69ms main cli] dfgram add 48.2 51", "dfgram add 48.2 51", "main");
    testPCL("[        main gui] @file Steca.log", "@file Steca.log", "main");
}
