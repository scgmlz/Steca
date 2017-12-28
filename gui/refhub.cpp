// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/refhub.cpp
//! @brief     Implements class RefHub
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "refhub.h"
#include "thehub.h"

namespace gui {

RefHub::RefHub(gui::TheHub& hub) : hub_(hub) {}

void RefHub::tellDatasetSelected(data::shp_Dataset d) { hub_.tellDatasetSelected(d); }
void RefHub::tellSelectedReflection(calc::shp_Reflection r) { hub_.tellSelectedReflection (r); }
void RefHub::tellReflectionData(calc::shp_Reflection r) { hub_.tellReflectionData(r); }
void RefHub::tellReflectionValues(
    typ::Range const& rge, qpair const& peak, fwhm_t fwhm, bool withGuesses) {
    hub_.tellReflectionValues(rge, peak, fwhm, withGuesses); }

} // namespace gui
