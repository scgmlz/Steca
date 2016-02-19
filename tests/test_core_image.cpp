#include "test_core_image.h"

void TestCoreImage::testImage(){
  //Testing if Data is corectly added and can be accessed again
  {
    QSize q(10,100);
    uint pos =42;
    float intents = 1.24;
    core::Image im;
    im.fill(intents,q);
    auto data = im.getIntensities();
    for(int i=0;i<1000;++i){
      auto d = *data;
      QCOMPARE(d,intents);
      data++;
    }
    im.setIntensity(pos,3*intents);
    QCOMPARE(*(im.getIntensities()+pos),3*intents);
    QCOMPARE(im.getRgeIntens().min,core::Range(intents,3*intents).min);
    QCOMPARE(im.getRgeIntens().max,core::Range(intents,3*intents).max);
    //checking if intensitys are corect
    for(int x=0;x<10;++x){
      for(int y=0;y<100;++y){
        if(x!=2 && y!=4){
          QCOMPARE(im.at(1,2),intents);
        }
        QCOMPARE(im.at(2,4),3*intents);
      }

    }
  }
}
