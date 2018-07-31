//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/params.h
//! @brief     Defines class Params
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PARAMS_H
#define PARAMS_H

#include "core/pars/detector.h"
#include "core/pars/image_transform.h"
#include "qcr/engine/param_wrapper.h"

//! Global user-selected parameters

class Params {
public:
    Detector             detector;
    ImageTransform       imageTransform;
    ImageCut             imageCut;
    ParamWrapper<bool>   intenScaledAvg {true}; // if not, summed
    ParamWrapper<double> intenScale {1};
};

#endif // PARAMS_H
