// ************************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/ij.cpp
//! @brief     Implements class IJ
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  Unit tests in test005_ij.
//
// ************************************************************************************************

#include "core/def/comparators.h"
#include "core/typ/ij.h"
#include "core/typ/json.h"

IJ::IJ()
    : IJ(0, 0)
{}

IJ::IJ(int i_, int j_)
    : i(i_), j(j_)
{}

int IJ::compare(const IJ& that) const
{
    RET_COMPARE_VALUE(i)
    RET_COMPARE_VALUE(j)
    return 0;
}

EQ_NE_OPERATOR(IJ)

QJsonObject IJ::toJson() const
{
    return { { "i", i }, { "j", j } };
}

void IJ::fromJson(const JsonObj& obj)
{
    i = obj.loadInt("i");
    j = obj.loadInt("j");
}
