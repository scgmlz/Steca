// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_help.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "dock_help.h"
#include "thehub.h"
#include <QTextBrowser>

namespace gui {
namespace panel {
//------------------------------------------------------------------------------

DockHelp::DockHelp(TheHub& hub) : super("Help", "dock-help", Qt::Vertical), RefHub(hub) {
    setFeatures(features() | DockWidgetFloatable);
    box_->addWidget((browser_ = new QTextBrowser()));
}

//------------------------------------------------------------------------------
}
}
// eof
