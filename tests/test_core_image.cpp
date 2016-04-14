#include "test_core_image.h"
#include "types/core_type_image.h"

void TestCoreImage::testImage(uint w, uint h) {
    //Testing if Data is corectly added and can be accessed again
    uint const specialX = qMin(2u,w-1), specialY = qMin(4u,h-1); // that's 42!

    ASSERT(specialX<w && specialY<h)

    QSize q(w,h);
    uint pos = specialX + w*specialY;
    float intens = 1.24, specialIntens = 3*intens;
    core::Image im;
    im.fill(intens,q);

    auto data = im.getIntensities();
    QCOMPARE(im.getCount(),w*h);
    for_i (im.getCount()) {
      QCOMPARE(*data++,intens);
    }

    im.setIntensity(pos,specialIntens);
    QCOMPARE(*(im.getIntensities()+pos),specialIntens);

    auto rgeActual = im.intensRange();
    auto rgeExpect = core::Range(intens,specialIntens);
    QCOMPARE(rgeActual.min, rgeExpect.min);
    QCOMPARE(rgeActual.max, rgeExpect.max);

    // checking if intensities are correct
    for (uint x=0; x<w; ++x) {
      for (uint y=0; y<h; ++y) {
        if (x==specialX && y==specialY) {
          QCOMPARE(im.at(x,y),specialIntens);
        } else {
          QCOMPARE(im.at(x,y),intens);
        }
      }
    }
}

void TestCoreImage::testImage() {
  testImage(10,100);
  testImage(256,256);
  testImage(256,25);
}

// eof
