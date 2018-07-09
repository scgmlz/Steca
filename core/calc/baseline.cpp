//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/baseline.cpp
//! @brief     Implements class Baseline
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/session.h"
#include "qcr/base/debug.h"

void Baseline::fromJson(const JsonObj obj)
{
    ranges_.fromJson(obj.loadArr("ranges"));
    polynomDegree.setVal(obj.loadUint("polynom degree"));
}

void Baseline::clear()
{
    polynomDegree.setVal(0);
    ranges_.clear();
}

void Baseline::clearRanges()
{
    ranges_.clear();
}

void Baseline::addRange(const Range& r)
{
    ranges_.add(r);
}

void Baseline::removeRange(const Range& r)
{
    ranges_.remove(r);
}

QJsonObject Baseline::toJson() const
{
    QJsonObject ret;
    ret.insert("polynom degree", polynomDegree.val());
    ret.insert("ranges", ranges().toJson());
    return ret;
}
