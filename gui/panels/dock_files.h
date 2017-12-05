// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_files.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DOCK_FILES_H
#define DOCK_FILES_H

#include "panel.h"

namespace gui {
namespace panel {
//------------------------------------------------------------------------------

class DockFiles : public DockWidget, private RefHub {
    CLASS(DockFiles) SUPER(DockWidget) public : DockFiles(TheHub&);

private:
    class FilesView* filesView_;
    class LineView* corrFile_;
};

//------------------------------------------------------------------------------
}
}
#endif // DOCK_FILES_H
