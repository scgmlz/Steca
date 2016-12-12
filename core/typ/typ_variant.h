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

#ifndef TYP_VARIANT_H
#define TYP_VARIANT_H

#include "typ/typ_vec.h"
#include <QVariant>

// Help with QVariant

namespace typ {
//------------------------------------------------------------------------------

bool isNumeric(QVariant const&);

// The usual comparators: <0, 0, >0
typedef int cmpFun(QVariant const&, QVariant const&);
typedef vec<cmpFun*> cmp_vec;

int cmp_int(QVariant const&,  QVariant const&);
int cmp_str(QVariant const&,  QVariant const&);
int cmp_real(QVariant const&, QVariant const&);
int cmp_date(QVariant const&, QVariant const&);

typedef vec<QVariant> row_t;

//------------------------------------------------------------------------------
}
#endif // TYP_VARIANT_H
