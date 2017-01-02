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

#include "typ_array2d.h"

#include "def/def_cmp_impl.h"
#include "typ/typ_json.h"

#include "test/tests.h"

namespace typ {
//------------------------------------------------------------------------------

int size2d::compare(rc that) const {
  RET_COMPARE_VALUE(w)
  RET_COMPARE_VALUE(h)
  return 0;
}

EQ_NE_OPERATOR(size2d)

//------------------------------------------------------------------------------

TEST("size2d", ({
  size2d sz;
  CHECK((0==sz.w && 0==sz.h && sz.isEmpty() && 0==sz.count()));

  size2d sz1(1,2);
  CHECK((1==sz1.w && 2==sz1.h && !sz1.isEmpty() && 2==sz1.count()));

  sz = sz1.transposed();
  CHECK((sz.w==sz1.h && sz.h==sz1.w));
});)

//------------------------------------------------------------------------------
}
// eof
