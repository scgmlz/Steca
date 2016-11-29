/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

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
