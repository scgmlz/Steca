#ifndef TESTS_MAIN_H
#define TESTS_MAIN_H

#include <QObject>
#include <QtTest/QtTest>

class TestSuite: public QObject {
  Q_OBJECT
public:
  TestSuite();

  static uint total, failed;

private slots:
  void cleanup();
};

class RegisterTestSuite {
public:
  RegisterTestSuite(TestSuite&);
};

#define REGISTER_TEST_SUITE(Suite)                  \
  static Suite suite;                               \
  static RegisterTestSuite _registeredSuite(suite); \

#endif // TESTS_MAIN_H
