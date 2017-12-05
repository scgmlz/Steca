// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/tabs_diffractogram.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TABS_DIFFRACTOGRAM_H
#define TABS_DIFFRACTOGRAM_H

#include "panel.h"

namespace gui {
namespace panel {

class TabsDiffractogram : public TabsPanel {
    CLASS(TabsDiffractogram)
    SUPER(TabsPanel) public : TabsDiffractogram(TheHub&);
};


}
}
#endif // TABS_DIFFRACTOGRAM_H
