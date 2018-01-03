// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_dataset.h
//! @brief     Defines class DockDatasets
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DOCK_DATASET_H
#define DOCK_DATASET_H

#include "widgets/various_widgets.h"

class QSpinBox;

namespace gui {
namespace panel {

class DockDatasets : public DockWidget {
public:
    DockDatasets();
    QSpinBox* combineDatasets_;

private:
    class DatasetView* dataseqView_;
};

} // namespace panel
} // namespace gui

#endif // DOCK_DATASET_H
