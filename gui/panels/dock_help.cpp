// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      dock_help.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "dock_help.h"
#include "thehub.h"
#include <QTextBrowser>

namespace gui { namespace panel {
//------------------------------------------------------------------------------

DockHelp::DockHelp(TheHub& hub)
: super("Help", "dock-help", Qt::Vertical), RefHub(hub)
{
  setFeatures(features() | DockWidgetFloatable);
  box_->addWidget((browser_ = new QTextBrowser()));
}

//------------------------------------------------------------------------------
}}
// eof
