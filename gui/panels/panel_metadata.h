// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      panel_metadata.h
//! @brief     Metadata panel.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef PANEL_METADATA_H
#define PANEL_METADATA_H

#include "panel.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class DockMetadata : public DockWidget, protected RefHub {
  CLS(DockMetadata) SUPER(DockWidget)
public:
  DockMetadata(TheHub&);

private:
  class MetadataView *metadataView_;
};

//------------------------------------------------------------------------------
}}
#endif // PANEL_METADATA_H
