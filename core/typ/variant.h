// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/variant.h
//! @brief     Defines helper functions related to QVariant
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

bool isNumeric(const QVariant&);

// The usual comparators: <0, 0, >0
typedef int cmpFun(const QVariant&, const QVariant&);
typedef vec<cmpFun*> cmp_vec;

int cmp_int(const QVariant&, const QVariant&);
int cmp_str(const QVariant&, const QVariant&);
int cmp_real(const QVariant&, const QVariant&);
int cmp_date(const QVariant&, const QVariant&);

typedef vec<QVariant> row_t;

#endif // VARIANT_H
