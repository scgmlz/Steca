// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_files.h
//! @brief     Defines class DockFiles
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DOCK_FILES_H
#define DOCK_FILES_H

#include "widgets/various_widgets.h"

namespace gui {
namespace panel {

class DockFiles : public DockWidget {
public:
    DockFiles();

private:
    class FilesView* filesView_;
    class LineView* corrFile_;
};

} // namespace panel
} // namespace gui

#endif // DOCK_FILES_H
