#ifndef TEST_APPROX_H
#define TEST_APPROX_H

#include <QtTest/QtTest>

class TestApprox: public QObject {
  Q_OBJECT

private slots:
  void testApproxFunction();
};

#endif
