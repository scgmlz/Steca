// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_variant.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_type_variant.h"
#include <QDate>
#include <QVariant>

namespace core {
//------------------------------------------------------------------------------

#define IMPL_CMP(name, toType)                       \
  int name(QVariant const& v1, QVariant const& v2) { \
    auto val1 = v1.toType(), val2 = v2.toType();     \
    if (val1 < val2) return -1;                      \
    if (val1 > val2) return +1;                      \
    return 0;                                        \
  }

IMPL_CMP(cmp_int,  toInt)
IMPL_CMP(cmp_str,  toString)
IMPL_CMP(cmp_real, toDouble)
IMPL_CMP(cmp_date, toDate)

#undef IMPL_CMP

//------------------------------------------------------------------------------
}
// eof
