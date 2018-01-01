// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/tabs_diffractogram.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "tabs_diffractogram.h"
#include "panel_diffractogram.h"

namespace gui {
namespace panel {

TabsDiffractogram::TabsDiffractogram() : super() {
    auto& box = addTab("Diffractogram", Qt::Vertical).box();
    box.addWidget(new Diffractogram());
}

}
}
