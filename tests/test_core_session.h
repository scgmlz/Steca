#ifndef TEST_CORE_SESSION_H
#define TEST_CORE_SESSION_H

#include <QtTest/QtTest>

#define TEST_UNIT_TESTS

class TestCoreSession: public QObject {
  Q_OBJECT

private slots:
  void testSession();
};

#endif
