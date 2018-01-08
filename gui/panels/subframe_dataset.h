// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_dataset.h
//! @brief     Defines class SubframeDatasets
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SUBFRAME_DATASET_H
#define SUBFRAME_DATASET_H

#include "gui/widgets/various_widgets.h"

class QSpinBox;

//! Part of the main window that controls the dataset selection.

//! Shows the datasets in the selected files, and allow to select some of them
//! and to combine them by a constant factor

class SubframeDatasets : public DockWidget {
public:
    SubframeDatasets();
    QSpinBox* combineDatasets_;

private:
    class DatasetView* dataseqView_;
};

#endif // SUBFRAME_DATASET_H
