// ************************************************************************** //
//
//  Steca2:    StressTextureCalculator ver. 2
//
//! @file      typ_variant.h
//! @brief     QVariant - derived
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_VARIANT_H
#define TYP_VARIANT_H

#include "typ/typ_vec.h"
#include <QVariant>

namespace typ {
//------------------------------------------------------------------------------

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
