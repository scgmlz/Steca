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
#include "qcr/engine/debug.h"

void Baseline::fromJson(const JsonObj obj)
{
    ranges_.fromJson(obj.loadArr("ranges"));
    polynomDegree.setParam(obj.loadUint("polynom degree"));
}

void Baseline::clear()
{
    polynomDegree.setParam(0);
    ranges_.clear();
}

void Baseline::setRanges(const Ranges& rr)
{
    ranges_ = rr;
    EMITS("Baseline::setRanges", gSession->sigBaseline());
}

void Baseline::addRange(const Range& r)
{
    ranges_.add(r);
    EMITS("Baseline::addRange", gSession->sigBaseline());
}

void Baseline::removeRange(const Range& r)
{
    ranges_.remove(r);
    EMITS("Baseline::removeRange", gSession->sigBaseline());
}

QJsonObject Baseline::toJson() const
{
    QJsonObject ret;
    ret.insert("polynom degree", polynomDegree.val());
    ret.insert("ranges", ranges().toJson());
    return ret;
}
