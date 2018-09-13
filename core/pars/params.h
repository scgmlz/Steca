//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/params.h
//! @brief     Defines and implements class Params
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
#include "core/typ/bool_vector.h"
#include "qcr/engine/cell.h"

enum class eNorm { NONE, MONITOR, DELTA_MONITOR, TIME, DELTA_TIME };
enum class EditableRange { NONE, BASELINE, PEAKS };

//! Global user-selected parameters

class Params {
public:
    Params() {}
    Params(const Params&) = delete;
    // void clear() { *this = {}; } TODO restore (broken because BoolVector disallows copying

    Detector        detector;
    ImageTransform  imageTransform;
    ImageCut        imageCut;

    QcrCell<bool>   intenScaledAvg {true}; // if not, summed
    QcrCell<double> intenScale {1.};
    QcrCell<int>    howtoNormalize {(int)eNorm::NONE};

    BoolVector      smallMetaSelection;  //!< for 'clusters' and 'metadata' subframes:
    BoolVector      bigMetaSelection;    //! for use in 'bigtable' (tabbed view and export):
    QcrCell<int>    diagramX {0};        //!< for use as x axis in diagram
    QcrCell<int>    diagramY {0};        //!< for use as y axis in diagram

    QcrCell<int>    defaultPeakFunction {1}; //!< Refers to Peak::keys.
    InterpolParams  interpolParams;

    EditableRange   editableRange{EditableRange::NONE};
    QcrCell<bool>   showAvgeDfgram {false};
};

#endif // PARAMS_H
