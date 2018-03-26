// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/state.cpp
//! @brief     Defines class GuiState
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "state.h"
#include "gui/base/controls.h"

GuiState::GuiState()
{
    polefigShowGridPts = new CCheckBox {"gridPts", "grid points"};
}
