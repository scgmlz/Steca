// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel_file.h
//! @brief     File selection panel.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef PANEL_FILE_H
#define PANEL_FILE_H

#include "panel.h"
#include "views.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class DockFiles: public DockWidget, private RefHub {
  SUPER(DockFiles,DockWidget)
public:
  DockFiles(TheHub&);

private:
  views::FilesView *filesView_;
  LineView  *corrFile_;
};

//------------------------------------------------------------------------------
}}
#endif
