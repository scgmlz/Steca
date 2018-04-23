//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/state.h
//! @brief     Defines class GuiState
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef STATE_H
#define STATE_H

#include <vector>

//! Collection of GUI state variables and controls.

class GuiState {
public:
    GuiState();
    GuiState(const GuiState&) = delete;
    bool editingBaseline {false};
    bool editingPeakfits {false};

    class QcrCheckBox* polefigShowGridPts;
    class QcrComboBox* diagramX;
    class QcrComboBox* diagramY;

    std::vector<bool> bigtableShowCol;
    const class BigtableModel* bigtableModel;
};

#endif // STATE_H
