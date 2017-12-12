// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/refhub.cpp
//! @brief     Implements classes TheHubSignallingBase and RefHub
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "thehub.h"
#include "typ/range.h"

namespace gui {

TheHub& TheHubSignallingBase::asHub() {
    ENSURE(dynamic_cast<TheHub*>(this))
    return *static_cast<TheHub*>(this);
}

void TheHubSignallingBase::tellSessionCleared() {
    emit sigSessionCleared();
}

void TheHubSignallingBase::tellDatasetSelected(data::shp_Dataset dataset) {
    emit sigDatasetSelected((asHub().selectedDataset_ = dataset));
}

void TheHubSignallingBase::tellSelectedReflection(calc::shp_Reflection reflection) {
    emit sigReflectionSelected((asHub().selectedReflection_ = reflection));
}

void TheHubSignallingBase::tellReflectionData(calc::shp_Reflection reflection) {
    emit sigReflectionData(reflection);
}

void TheHubSignallingBase::tellReflectionValues(
    typ::Range const& rgeTth, qpair const& peak, fwhm_t fwhm, bool withGuesses) {
    emit sigReflectionValues(rgeTth, peak, fwhm, withGuesses);
}

TheHubSignallingBase::level_guard::level_guard(level_t& level) : level_(level) {
    ++level_;
}

TheHubSignallingBase::level_guard::~level_guard() {
    --level_;
}


RefHub::RefHub(gui::TheHub& hub) : hub_(hub) {}

void RefHub::tellDatasetSelected(data::shp_Dataset d) { hub_.tellDatasetSelected(d); }
void RefHub::tellSelectedReflection(calc::shp_Reflection r) { hub_.tellSelectedReflection (r); }
void RefHub::tellReflectionData(calc::shp_Reflection r) { hub_.tellReflectionData(r); }
void RefHub::tellReflectionValues(
    typ::Range const& rge, qpair const& peak, fwhm_t fwhm, bool withGuesses) {
    hub_.tellReflectionValues(rge, peak, fwhm, withGuesses); }

} // namespace gui
