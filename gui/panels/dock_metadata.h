// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_metadata.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DOCK_METADATA_H
#define DOCK_METADATA_H

#include "panel.h"

namespace gui {
namespace panel {
//------------------------------------------------------------------------------

class DockMetadata : public DockWidget, protected RefHub {
    CLASS(DockMetadata) SUPER(DockWidget) public : DockMetadata(TheHub&);

private:
    class MetadataView* metadataView_;
};

//------------------------------------------------------------------------------
}
}
#endif // DOCK_METADATA_H
