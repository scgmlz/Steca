#ifndef TEST_CORE_FUNCTIONS_H
#define TEST_CORE_FUNCTIONS_H

#include <QtTest/QtTest>

class TestCoreApprox: public QObject {
  Q_OBJECT

private slots:
  void testApproxFunction();
  void testPolynomial();
};

#endif // TEST_CORE_FUNCTIONS_H
