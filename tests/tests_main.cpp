// All that we reasonably care to test, and then some.

#include "tests_main.h"

uint TestSuite::total(0);
uint TestSuite::failed(0);

TestSuite::TestSuite() {
}

void TestSuite::cleanup() {
  ++total;
  if (QTest::currentTestFailed())
    ++failed;
}

static vec<TestSuite*>& tests() {
  static vec<TestSuite*> tests_;
  return tests_;
}

RegisterTestSuite::RegisterTestSuite(TestSuite& testSuite) {
  tests().append(&testSuite);
}

#include <QTextStream>

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  QTextStream out(stdout);

  int status = 0;

  for (auto test: tests()) {
    status |= QTest::qExec(test);
    endl(out);                                              \
  }

  out << "Result: " << TestSuite::total << " tests, " << TestSuite::failed << " failed" << endl;
  if (0 != status) out << "!!** Some tests failed **!!" << endl;

  return status;
}

// eof
