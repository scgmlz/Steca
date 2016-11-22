// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      dock_metadata.h
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef DOCK_HELP_H
#define DOCK_HELP_H

#include "panel.h"

class QTextBrowser;

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class DockHelp : public DockWidget, protected RefHub {
  CLS(DockHelp) SUPER(DockWidget)
public:
  DockHelp(TheHub&);

private:
  QTextBrowser *browser_;
};

//------------------------------------------------------------------------------
}}
#endif // DOCK_HELP_H
