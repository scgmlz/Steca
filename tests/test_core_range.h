#ifndef TEST_CORE_RANGE_H
#define TEST_CORE_RANGE_H

#include <QtTest/QtTest>

#define TEST_UNIT_TESTS

class TestCoreRange: public QObject {
  Q_OBJECT

private slots:
  void testRange();
};

#endif
