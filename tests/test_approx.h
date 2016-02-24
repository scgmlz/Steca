#ifndef TEST_APPROX_H
#define TEST_APPROX_H

#include <QtTest/QtTest>

#define TEST_UNIT_TESTS

class TestApprox: public QObject {
  Q_OBJECT

private slots:
  void testApproxFunction();
};

#endif
