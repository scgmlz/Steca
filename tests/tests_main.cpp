#include "test_core_lib.h"
#include "test_core_array2d.h"
#include "test_core_image.h"
#include "test_core_range.h"
#include "test_core_session.h"
#include "test_save_load_json.h"
#include "test_link.h"
#include "test_approx.h"

#define TEST_SUITE(TestClass) {    \
  TestClass test;                  \
  QTest::qExec(&test, argc, argv); \
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  // test suites
  TEST_SUITE(TestApprox)
  TEST_SUITE(TestCoreArray2d)
  TEST_SUITE(TestCoreImage)
  TEST_SUITE(TestCoreLib)
  TEST_SUITE(TestCoreRange)
  TEST_SUITE(TestCoreSession)
  TEST_SUITE(TestPriorityChain)
  TEST_SUITE(TestSaveLoadJson)
}

// eof
