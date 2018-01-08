// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/variant.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef VARIANT_H
#define VARIANT_H

#include "core/typ/vec.h"

// Help with QVariant

bool isNumeric(QVariant const&);

// The usual comparators: <0, 0, >0
typedef int cmpFun(QVariant const&, QVariant const&);
typedef vec<cmpFun*> cmp_vec;

int cmp_int(QVariant const&, QVariant const&);
int cmp_str(QVariant const&, QVariant const&);
int cmp_real(QVariant const&, QVariant const&);
int cmp_date(QVariant const&, QVariant const&);

typedef vec<QVariant> row_t;

#endif // VARIANT_H
