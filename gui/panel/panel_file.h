// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel_file.h
//! @brief     File panel.
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
#include "core_image.h"
#include "core_dataset.h"
#include "models.h"

namespace panel {
//------------------------------------------------------------------------------

class FileView: public HubListView {
  SUPER(FileView,HubListView)
public:
  using Model = model::FileViewModel;

  FileView(TheHub&);

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

public:
  void removeSelected();
  void update();

private:
  Model &model;
};

class DockFiles: public DockWidget {
  SUPER(DockFiles,DockWidget)
public:
  DockFiles(TheHub&);
private:
  FileView *fileView;
};

//------------------------------------------------------------------------------
}
#endif
