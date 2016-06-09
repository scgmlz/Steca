#ifndef TEST_CORE_IMAGE_H
#define TEST_CORE_IMAGE_H

#include "tests_main.h"

class TestCoreImage: public TestSuite {
  Q_OBJECT

  void testImage(uint w, uint h);

private slots:
  void testImage();
};

#endif
