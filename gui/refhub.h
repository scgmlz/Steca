// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/refhub.h
//! @brief     Defines class RefHub
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef REFHUB_H
#define REFHUB_H

#include "calc/calc_reflection.h"
#include "data/datafile.h"

namespace gui {

class TheHub;

//! The base class that refers to the hub + support methods.

class RefHub {
public:
    RefHub(TheHub&);
protected:
    TheHub& hub_;
protected:
    void tellDatasetSelected(data::shp_Dataset);
    void tellSelectedReflection(calc::shp_Reflection);
    void tellReflectionData(calc::shp_Reflection);
    void tellReflectionValues(typ::Range const&, qpair const&, fwhm_t, bool);
};

} //namespace gui

#endif // REFHUB_H
