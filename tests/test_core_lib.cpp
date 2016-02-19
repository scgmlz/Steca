#include "test_core_lib.h"

void TestCoreLib::conversions() {
  QCOMPARE(M_PI,core::rad_deg(180));
  QCOMPARE(90.0,core::deg_rad(M_PI_2));
  for_i(720)
    QCOMPARE((qreal)i,core::deg_rad(core::rad_deg(i)));
}

void TestCoreLib::testFile(){
  {//using default Constructor
    core::File f;
    QByteArray b;
    QCOMPARE(f.getImageSize(),QSize(0,0));
  }
}

void TestCoreLib::testSession(){
  //Image transform
  {
    //Rectangle
    rotationHelper(10,20);
    //Square
    rotationHelper(10,10);
  }
  //Session
  {
    core::Session s;
    s.addFile("");//expect no exception because filename not given
    rcstr name = "TestFile";
    core::File f(name);
    //Filetype is not know, expected to throw
    bool check = false;
    try{
      s.addFile(name);
    }
    catch(Exception e){
        check=true;
    }
    QVERIFY(check);
  }
}

#define TEST_DATA(p1,p2,p3,p4) \
  QCOMPARE(a.at(s.pixIndex(0,0)),p1); \
  QCOMPARE(a.at(s.pixIndex(w-1,0)),p2);\
  QCOMPARE(a.at(s.pixIndex(w-1,h-1)),p3);\
  QCOMPARE(a.at(s.pixIndex(0,h-1)),p4);

void TestCoreLib::rotationHelper(uint w, uint h){
    QSize size(w,h);
    core::Array2D<int>a;
    a.fill(0,size);
    a.setAt(a.index(0,0),1);
    a.setAt(a.index(w-1,0),2);
    a.setAt(a.index(w-1,h-1),3);
    a.setAt(a.index(0,h-1),4);
    core::Session s;
    s.setImageSize(size);
    {
      //only rotation
      auto tb = core::Session::ImageTransform(0);
      s.setImageRotate(tb);
      //rotate_0
      TEST_DATA(1,2,3,4)
      //rotate_2
      tb = core::Session::ImageTransform(2);
      s.setImageRotate(tb);
      TEST_DATA(3,4,1,2)
      transposeWH(&w,&h);//flip w and h
      //rotate_1
      tb = core::Session::ImageTransform(1);
      s.setImageRotate(tb);
      TEST_DATA(4,1,2,3)
      //rotate_3
      tb = core::Session::ImageTransform(3);
      s.setImageRotate(tb);
      TEST_DATA(2,3,4,1)
    }
    transposeWH(&w,&h);//reset w and h
    {
      //mirror and rotation
      //mirror_rotate_0
      s.setImageMirror(true);
      auto tb = core::Session::ImageTransform(0);
      s.setImageRotate(tb);
      TEST_DATA(2,1,4,3)
      //mirror_rotate_2
      tb = core::Session::ImageTransform(2);
      s.setImageRotate(tb);
      TEST_DATA(4,3,2,1)
      transposeWH(&w,&h); //flip w and h
      //mirror_rotate_1
      tb = core::Session::ImageTransform(1);
      s.setImageRotate(tb);
      TEST_DATA(3,2,1,4)
      //mirror_rotate_3
      tb = core::Session::ImageTransform(3);
      s.setImageRotate(tb);
      TEST_DATA(1,4,3,2)
    }
}

void TestCoreLib::transposeWH(uint* w, uint* h){
    int hOrig = *h;
    int wOrig = *w;
    *w = hOrig;
    *h = wOrig;
}
