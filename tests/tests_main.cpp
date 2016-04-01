#include "test_core_lib.h"
#include "test_core_array2d.h"
#include "test_core_dataset.h"
#include "test_core_image.h"
#include "test_core_range.h"
#include "test_core_session.h"
#include "test_save_load_json.h"
#include "test_core_link.h"
#include "test_core_functions.h"
#include "test_core_lens.h"
#include "test_core_polefigure.h"
#include <QTextStream>

bool failed = false;

#define TEST_SUITE(TestClass)                              \
{                                                          \
    TestClass test;                                        \
    bool success = (0 == QTest::qExec(&test, argc, argv)); \
    failed       = failed || !success;                     \
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  // test suites
  TEST_SUITE(TestCoreApprox)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreArray2d)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreDataset)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreImage)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreLens)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCorePriorityChain)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreLib)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreRange)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCoreSession)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestSaveLoadJson)
  QTextStream(stdout) << "" << endl;
  TEST_SUITE(TestCorePolefigure)

  if (failed) qDebug() << "!! Some tests failed !!";
  return failed ? -1 : 0;
}

// eof
