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

} // namespace gui
