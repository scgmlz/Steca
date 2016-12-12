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

#ifndef DEF_COMPARE_H
#define DEF_COMPARE_H

// a helper for int compare()
#define COMPARE_VALUE(val)        \
  if (val < that.val) return -1;  \
  if (val > that.val) return +1;

#define COMPARE_VALUE2(v1,v2)     \
  if (v1 < v2) return -1;         \
  if (v1 > v2) return +1;

#define COMPARE_COMPARABLE(o) \
  for (int cmp = o.compare(that.o); cmp; ) \
    return cmp;

#endif // DEF_COMPARE_H
