#ifndef TEST_CORE_LIB_H
#define TEST_CORE_LIB_H

#include <QtTest/QtTest>

#define TEST_SUITE

#include <core_lib.h>
#include <core_array2d.h>
#include <core_debug.h>
#include <core_image.h>
#include <core_file.h>
#include <core_session.h>

class TestCoreLib: public QObject {
  Q_OBJECT
  void rotationHelper(uint w, uint h);
  void transposeWH(uint* w, uint* h);

private slots:
  void testArray2d();
  void testLib();
  void testImage();
  void conversions();
  void testFile();
  void testSession();
};

#endif
