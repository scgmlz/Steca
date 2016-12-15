/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "typ/typ_ij.h"
#include "def/def_cmp_impl.h"
#include "typ/typ_json.h"
#include "test/tests.h"

namespace typ {
//------------------------------------------------------------------------------

IJ::IJ() : IJ(0, 0) {}

TEST("IJ()", ({
  IJ ij;
  CHECK_EQ(0, ij.i);
  CHECK_EQ(0, ij.j);
});)

IJ::IJ(int i_, int j_) : i(i_), j(j_) {}

TEST("IJ(i,j)", ({
  IJ ij(2,3);
  CHECK_EQ(2, ij.i);
  CHECK_EQ(3, ij.j);
});)

int IJ::compare(rc that) const {
  RET_COMPARE_VALUE(i)
  RET_COMPARE_VALUE(j)
  return 0;
}

EQ_NE_OPERATOR(IJ)

TEST("IJ::compare", ({
  IJ ij(1,2), ij1(1,2), ij2(1,0), ij3(2,2);
  CHECK_EQ( 0, ij.compare(ij));
  CHECK_EQ( 0, ij.compare(ij1));
  CHECK_EQ( 1, ij.compare(ij2));
  CHECK_EQ(-1, ij.compare(ij3));

  CHECK_EQ(ij, ij1);
  CHECK_NE(ij, ij2);
});)

JsonObj IJ::saveJson() const {
  return JsonObj().saveInt(json_key::I, i).saveInt(json_key::J, j);
}

void IJ::loadJson(JsonObj::rc obj) THROWS {
  i = obj.loadInt(json_key::I);
  j = obj.loadInt(json_key::J);
}

TEST("IJ::json", ({
  IJ ij(-1,2), ij1;
  ij1.loadJson(ij.saveJson());
  CHECK_EQ(ij, ij1);
});)

//------------------------------------------------------------------------------
}
// eof
