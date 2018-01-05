// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/table_model.cpp
//! @brief     Implements class TableModel
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "table_model.h"

QVariant const EMPTY_VAR;
QModelIndex const ANY_INDEX;

void TableModel::signalReset() {
    beginResetModel();
    endResetModel();
}
