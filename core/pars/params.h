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
#include "core/pars/interpol_params.h"
#include "core/meta/meta_selection.h"
#include "qcr/engine/cell.h"

enum class eNorm {
    NONE,
    MONITOR,
    DELTA_MONITOR,
    TIME,
    DELTA_TIME,
};

//! Global user-selected parameters

class Params {
public:
    Params() {}
    Params(const Params&) = delete;
    // void clear() { *this = {}; } TODO restore (broken because MetaSelection disallows copying

    Detector        detector;
    ImageTransform  imageTransform;
    ImageCut        imageCut;
    InterpolParams  interpolParams;
    QcrCell<bool>   intenScaledAvg {true}; // if not, summed
    QcrCell<double> intenScale {1.};
    eNorm           normMode {eNorm::NONE};
    MetaSelection   smallMetaSelection {false}; //!< for 'clusters' and 'metadata' subframes
    MetaSelection   bigMetaSelection {true};    //!< for use in 'bigtable' (tabbed view and export)
};

#endif // PARAMS_H
