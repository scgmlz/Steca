//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/baseline.cpp
//! @brief     Implements class Baseline
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/pars/baseline.h"
#include "core/session.h"
//#include "qcr/base/debug.h"

Baseline::Baseline()
{
    polynomDegree.setHook([](int){gSession->onBaseline();});
}

void Baseline::fromJson(const JsonObj obj)
{
    ranges.fromJson(obj.loadArr("ranges"));
    polynomDegree.setVal(obj.loadUint("polynom degree"));
    gSession->onBaseline();
}

void Baseline::clear()
{
    ranges.clear();
    gSession->onBaseline();
}

void Baseline::removeSelected()
{
    ranges.removeSelected();
    gSession->onBaseline();
}

QJsonObject Baseline::toJson() const
{
    QJsonObject ret;
    ret.insert("ranges", ranges.toJson());
    ret.insert("polynom degree", polynomDegree.val());
    return ret;
}
