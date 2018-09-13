//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/aux/variant.h
//! @brief     Defines helper functions related to QVariant
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef VARIANT_H
#define VARIANT_H

#include <QVariant>

bool isNumeric(const QVariant&);

// The usual comparators: <0, 0, >0
typedef int VariantComparator(const QVariant&, const QVariant&);

int cmp_int(const QVariant&, const QVariant&);
int cmp_str(const QVariant&, const QVariant&);
int cmp_real(const QVariant&, const QVariant&);
int cmp_date(const QVariant&, const QVariant&);

#endif // VARIANT_H
