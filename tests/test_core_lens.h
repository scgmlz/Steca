#ifndef TEST_CORE_LENS_H
#define TEST_CORE_LENS_H

#include "core_lens.h"
#include "core_dataset.h"
#include "core_angle_map_array.h"

#include <QtTest/QtTest>

#define TEST_UNIT_TESTS

class TestCoreLens: public QObject {
  Q_OBJECT
public:
  void checkIntensity(core::shp_LensSystem const& lensSystem,
                      core::intens_t& val, core::intens_t& specialVal, int posX, int posY);
private slots:
  void testLensSystem();
};


#endif // TEST_CORE_LENS_H
