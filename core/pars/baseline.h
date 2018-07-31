//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/baseline.h
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
    void fromJson(const JsonObj obj);
    void clear();

    QJsonObject toJson() const;

    Ranges ranges;
    ParamWrapper<int> polynomDegree {0};
};

#endif // BASELINE_H
