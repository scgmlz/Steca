//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/baseline.h
//! @brief     Defines class Baseline
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef BASELINE_H
#define BASELINE_H

#include "core/typ/range.h"
#include "qcr/engine/param_wrapper.h"

//! Settings needed for fitting the baseline.

class Baseline {
public:
    void clear();

    void fromJson(const JsonObj obj);
    QJsonObject toJson() const;

    const Ranges& ranges() const { return ranges_; }
    Ranges& ranges() { return ranges_; }

    NumberWrapper<int> polynomDegree {0};

private:
    Ranges ranges_;
};

#endif // BASELINE_H
