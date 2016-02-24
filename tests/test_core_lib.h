#ifndef TEST_CORE_LIB_H
#define TEST_CORE_LIB_H

#include <QtTest/QtTest>

#define TEST_UNIT_TESTS

class TestCoreLib: public QObject {
  Q_OBJECT

  void rotationHelper(uint w, uint h);

private slots:
  void testConversions();
  void testFile();
  void testSession();
};

#endif
