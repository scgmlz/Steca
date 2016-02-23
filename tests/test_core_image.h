#ifndef TEST_CORE_IMAGE_H
#define TEST_CORE_IMAGE_H

#include <QtTest/QtTest>

#define TEST_SUITE

class TestCoreImage: public QObject {
  Q_OBJECT

  void testImage(uint w, uint h);

private slots:

  void testImage();
};

#endif
