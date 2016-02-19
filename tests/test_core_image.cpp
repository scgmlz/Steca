#include "test_core_image.h"
#include <core_image.h>

void TestCoreImage::testImage() {
  //Testing if Data is corectly added and can be accessed again
  {
    QSize q(10,100);
    uint pos = 42;
    float intens = 1.24;
    core::Image im;
    im.fill(intens,q);

    auto data = im.getIntensities();
    for_i (1000) {
      QCOMPARE(*data++,intens);
    }

    im.setIntensity(pos,3*intens);
    QCOMPARE(*(im.getIntensities()+pos),3*intens);
    QCOMPARE(im.getRgeIntens().min,core::Range(intens,3*intens).min);
    QCOMPARE(im.getRgeIntens().max,core::Range(intens,3*intens).max);

    //checking if intensities are corect
    for (int x=0;x<10;++x) {
      for (int y=0;y<100;++y) {
        if(x!=2 && y!=4) {
          QCOMPARE(im.at(1,2),intens);
        }
        QCOMPARE(im.at(2,4),3*intens);
      }
    }
  }
}

// eof
