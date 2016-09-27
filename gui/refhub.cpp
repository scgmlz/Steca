// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      refhub.h
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "refhub.h"
#include "thehub.h"

namespace gui {
//------------------------------------------------------------------------------

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
    tth_rge::rc rgeTth, peak_t::rc peak, fwhm_t fwhm, bool withGuesses) {
  emit sigReflectionValues(rgeTth, peak, fwhm, withGuesses);
}

TheHubSignallingBase::level_guard::level_guard(level_t& level) : level_(level) {
  ++level_;
}

TheHubSignallingBase::level_guard::~level_guard() {
  --level_;
}

//------------------------------------------------------------------------------

RefHub::RefHub(gui::TheHub& hub) : hub_(hub) {
}

#define REFHUB_TELL_IMPL(name, pars, args) \
  void RefHub::name pars { hub_.name args; }

REFHUB_TELL_IMPL(tellDatasetSelected, (data::shp_Dataset d), (d))

REFHUB_TELL_IMPL(tellSelectedReflection, (calc::shp_Reflection r), (r))

REFHUB_TELL_IMPL(tellReflectionData, (calc::shp_Reflection r), (r))

REFHUB_TELL_IMPL(tellReflectionValues,
    (typ::Range::rc rge, typ::XY::rc peak, fwhm_t fwhm, bool withGuesses),
    (rge, peak, fwhm, withGuesses))

//------------------------------------------------------------------------------
}
// eof
