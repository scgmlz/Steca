// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_diffractogram.cpp
//! @brief     Implements class SubframeDiffractogram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/panels/subframe_diffractogram.h"
#include "gui/panels/panel_diffractogram.h"
#include "gui/base/various_widgets.h"
#include "gui/base/new_q.h"

SubframeDiffractogram::SubframeDiffractogram() {
    setTabPosition(QTabWidget::North);
    newQ::Tab(this, "Diffractogram")->box().addWidget(new Diffractogram());
}
