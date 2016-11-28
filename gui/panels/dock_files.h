// ************************************************************************** //
//
// STeCa2 - StressTextureCalculator ver. 2
//
// Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the COPYING and AUTHORS files for more details.
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
