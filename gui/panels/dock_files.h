// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      dock_files.h
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef DOCK_FILES_H
#define DOCK_FILES_H

#include "panel.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class DockFiles : public DockWidget, private RefHub {
  CLS(DockFiles) SUPER(DockWidget)
public:
  DockFiles(TheHub&);

private:
  class FilesView *filesView_;
  class LineView * corrFile_;
};

//------------------------------------------------------------------------------
}}
#endif // DOCK_FILES_H
