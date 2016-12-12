/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
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

#include "typ_ij.h"
#include "../def/def_compare.h"
#include "typ_json.h"

namespace typ {
//------------------------------------------------------------------------------

IJ::IJ() : IJ(0, 0) {}

IJ::IJ(int i_, int j_) : i(i_), j(j_) {}

int IJ::compare(rc that) const {
  COMPARE_VALUE(i)
  COMPARE_VALUE(j)
  return 0;
}

JsonObj IJ::saveJson() const {
  return JsonObj().saveInt(json_key::I, i).saveInt(json_key::J, j);
}

void IJ::loadJson(JsonObj::rc obj) THROWS {
  i = obj.loadInt(json_key::I);
  j = obj.loadInt(json_key::J);
}

//------------------------------------------------------------------------------
}
// eof
