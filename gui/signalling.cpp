// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/signalling.cpp
//! @brief     Implements class TheHubSignallingBase
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "signalling.h"
#include "thehub.h"

namespace gui {

class TheHub;

TheHub& TheHubSignallingBase::asHub() {
    debug::ensure(dynamic_cast<TheHub*>(this));
    return *static_cast<TheHub*>(this);
}

void TheHubSignallingBase::tellSessionCleared() {
    emit sigSessionCleared();
}

void TheHubSignallingBase::tellSuiteSelected(QSharedPointer<Suite> dataseq) {
    emit sigDatasetSelected((asHub().selectedDataset_ = dataseq));
}

void TheHubSignallingBase::tellSelectedReflection(calc::shp_Reflection reflection) {
    emit sigReflectionSelected((asHub().selectedReflection_ = reflection));
}

void TheHubSignallingBase::tellReflectionData(calc::shp_Reflection reflection) {
    emit sigReflectionData(reflection);
}

void TheHubSignallingBase::tellReflectionValues(
    Range const& rgeTth, qpair const& peak, fwhm_t fwhm, bool withGuesses) {
    emit sigReflectionValues(rgeTth, peak, fwhm, withGuesses);
}


TheHubSignallingBase::level_guard::level_guard(uint& level) : level_(level) {
    ++level_;
}

TheHubSignallingBase::level_guard::~level_guard() {
    --level_;
}

} // namespace gui
