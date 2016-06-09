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

void TheHubSignallingBase::tellDatasetSelected(core::shp_Dataset dataset) {
  emit sigDatasetSelected((asHub().selectedDataset_ = dataset));
}

void TheHubSignallingBase::tellSelectedReflection(core::shp_Reflection reflection) {
  emit sigReflectionSelected((asHub().selectedReflection_ = reflection));
}

void TheHubSignallingBase::tellReflectionData(core::shp_Reflection reflection) {
  emit sigReflectionData(reflection);
}

void TheHubSignallingBase::tellReflectionValues(core::rcRange range,
                                                core::rcXY peak, qreal fwhm,
                                                bool withGuesses) {
  emit sigReflectionValues(range, peak, fwhm, withGuesses);
}

TheHubSignallingBase::level_guard::level_guard(level_t& level) : level_(level) {
  ++level_;
}

TheHubSignallingBase::level_guard::~level_guard() {
  --level_;
}

//------------------------------------------------------------------------------

RefHub::RefHub(gui::TheHub& hub) : hub_(hub) {}

#define REFHUB_TELL_IMPL(name, pars, args) \
  void RefHub::name pars { hub_.name args; }

REFHUB_TELL_IMPL(tellDatasetSelected, (core::shp_Dataset d), (d))

REFHUB_TELL_IMPL(tellSelectedReflection, (core::shp_Reflection r), (r))

REFHUB_TELL_IMPL(tellReflectionData, (core::shp_Reflection r), (r))

REFHUB_TELL_IMPL(tellReflectionValues,
(core::rcRange rge, core::rcXY xy, qreal r, bool b),
(rge, xy, r, b))

//------------------------------------------------------------------------------
}
// eof
