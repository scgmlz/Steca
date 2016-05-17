// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
//
//! @file      core_type_variant.h
//! @brief     QVariant - derived
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_TYPE_VARIANT_H
#define CORE_TYPE_VARIANT_H

#include "core_defs.h"
#include "core_types_fwd.h"

namespace core {
//------------------------------------------------------------------------------

/// The usual comparators: <0, 0, >0
typedef int cmpFun(QVariant const&, QVariant const&);
typedef QVector<cmpFun*> cmp_vec;

int cmp_int(QVariant const&,  QVariant const&);
int cmp_str(QVariant const&,  QVariant const&);
int cmp_real(QVariant const&, QVariant const&);
int cmp_date(QVariant const&, QVariant const&);

typedef QVector<QVariant> row_t;

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_VARIANT_H
