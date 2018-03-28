// ************************************************************************** //
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
// ************************************************************************** //

#include "core/session.h"

void Baseline::fromJson(const JsonObj obj)
{
    ranges_.fromJson(obj.loadArr("ranges"));
    polynomDegree_ = obj.loadUint("polynom degree");
}

void Baseline::clear()
{
    polynomDegree_ = 0;
    ranges_.clear();
}

void Baseline::setRanges(const Ranges& rr)
{
    ranges_ = rr;
    emit gSession->sigBaseline();
}

void Baseline::addRange(const Range& r)
{
    ranges_.add(r);
    emit gSession->sigBaseline();
}

void Baseline::removeRange(const Range& r)
{
    ranges_.remove(r);
    emit gSession->sigBaseline();
}

void Baseline::setPolynomDegree(int degree)
{
    polynomDegree_ = degree;
    emit gSession->sigBaseline();
}

QJsonObject Baseline::toJson() const
{
    QJsonObject ret;
    ret.insert("polynom degree", polynomDegree());
    ret.insert("ranges", ranges().toJson());
    return ret;
}
