// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_dataset.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DOCK_DATASET_H
#define DOCK_DATASET_H

#include "panel.h"

namespace gui {
namespace panel {

class DockDatasets : public DockWidget, protected RefHub {
    CLASS(DockDatasets) SUPER(DockWidget) public : DockDatasets(TheHub&);

    QSpinBox* combineDatasets_;

private:
    class DatasetView* datasetView_;
};


}
}
#endif // DOCK_DATASET_H
