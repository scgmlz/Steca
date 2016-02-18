#ifndef TEST_CORE_ARRAY2D_H
#define TEST_CORE_ARRAY2D_H

#include <QtTest/QtTest>

#define TEST_SUITE
#include <core_lib.h>
#include <core_array2d.h>

class TestCoreArray2d: public QObject {
  Q_OBJECT

private slots:
  void testArray2d();
};

#endif
