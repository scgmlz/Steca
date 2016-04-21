#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "core_dataset.h"



class TestHelpers {
public:
  TestHelpers();
  
  core::Array2D<qreal> const testCorrArray(qreal corrVal,int width, int height);
  core::Dataset const testDataset(QSize size, core::inten_t inten, QVector<qreal> motorAngles, qreal mon, qreal deltaTime);
};

#endif // TEST_HELPERS_H
