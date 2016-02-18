#ifndef TEST_CORE_IMAGE_H
#define TEST_CORE_IMAGE_H

#include <QtTest/QtTest>

#define TEST_SUITE
#include <core_lib.h>
#include <core_array2d.h>
#include <core_image.h>

class TestCoreImage: public QObject {
  Q_OBJECT

private slots:
void testImage();


};



#endif // TEST_CORE_IMAGE_H
