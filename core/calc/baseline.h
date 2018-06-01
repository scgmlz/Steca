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

#include "qcr/engine/param_wrapper.h"
#include "core/typ/range.h"

//! Settings needed for fitting the baseline.

class Baseline {
public:
    void fromJson(const JsonObj obj);
    void clear();
    void setRanges(const Ranges&);
    void addRange(const Range&);
    void removeRange(const Range&);
    void setPolynomDegree(int);

    QJsonObject toJson() const;
    const Ranges& ranges() const { return ranges_; }
    NumberWrapper<int> polynomDegree {0};

private:
    Ranges ranges_;
};

#endif // BASELINE_H
