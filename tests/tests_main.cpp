#include "test_core_lib.h"
#include "test_core_array2d.h"
#include "test_core_image.h"
#include "test_core_range.h"
#include "test_core_session.h"
#include "test_save_load_json.h"
#include "test_link.h"
#include "test_approx.h"
#include "test_core_lens.h"
#include <QTextStream>

#define TEST_SUITE(TestClass) {    \
  TestClass test;                  \
  QTest::qExec(&test, argc, argv); \
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  // test suites
  TEST_SUITE(TestApprox)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreArray2d)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreImage)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreLens)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestPriorityChain)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreLib)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreRange)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreSession)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestSaveLoadJson)
}

// eof
