// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/typ/typ_variant.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_VARIANT_H
#define TYP_VARIANT_H

#include "typ/typ_vec.h"
#include <QVariant>

// Help with QVariant

namespace typ {

bool isNumeric(QVariant const&);

// The usual comparators: <0, 0, >0
typedef int cmpFun(QVariant const&, QVariant const&);
typedef vec<cmpFun*> cmp_vec;

int cmp_int(QVariant const&, QVariant const&);
int cmp_str(QVariant const&, QVariant const&);
int cmp_real(QVariant const&, QVariant const&);
int cmp_date(QVariant const&, QVariant const&);

typedef vec<QVariant> row_t;
}
#endif // TYP_VARIANT_H
