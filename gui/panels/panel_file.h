// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      panel_file.h
//! @brief     File selection panel.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef PANEL_FILE_H
#define PANEL_FILE_H

#include "gui_helpers.h"
#include "refhub.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class DockFiles : public DockWidget, private RefHub {
  SUPER(DockFiles, DockWidget)
public:
  DockFiles(TheHub&);

private:
  class FilesView *filesView_;
  class LineView * corrFile_;
};

//------------------------------------------------------------------------------
}}
#endif
