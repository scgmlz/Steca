#ifndef TEST_CORE_RANGE_H
#define TEST_CORE_RANGE_H

#include <QtTest/QtTest>

#define TEST_SUITE

#include <core_lib.h>


class TestCoreRange: public QObject {
  Q_OBJECT


private slots:
  void testRange();

};

#endif
