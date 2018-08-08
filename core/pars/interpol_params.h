//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/interpol_params.h
//! @brief     Defines InterpolParams
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef INTERPOL_PARAMS_H
#define INTERPOL_PARAMS_H

#include "core/typ/json.h"
#include "qcr/engine/cell.h"

//! Supports different ways of setting a gamma range.

class InterpolParams {
public:
    InterpolParams();
    ~InterpolParams();

    void fromJson(const JsonObj& obj);

    QJsonObject toJson() const;

    QcrCell<bool>   enabled     {false};
    QcrCell<double> stepAlpha   {5.};
    QcrCell<double> stepBeta    {5.};
    QcrCell<double> idwRadius   {10.};
    QcrCell<double> avgAlphaMax {5.};
    QcrCell<double> avgRadius   {5.};
    QcrCell<int>    threshold   {100};

};

#endif // INTERPOL_PARAMS_H
