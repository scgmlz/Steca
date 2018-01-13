// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/models.cpp
//! @brief     Implements classes TableModel, FilesModel, MeasurementsModel, MetadataModel, ReflectionsModel
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/models.h"
#include "core/fit/fit_fun.h"
#include "core/session.h"
#include "gui/thehub.h"

// ************************************************************************** //
//  class TableModel
// ************************************************************************** //

void TableModel::signalReset() {
    beginResetModel();
    endResetModel();
}
