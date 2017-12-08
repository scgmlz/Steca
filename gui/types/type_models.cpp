// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/types/type_models.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "type_models.h"
#include "typ/str.h"

namespace models {

QVariant const EMPTY_VAR;
QModelIndex const ANY_INDEX;

// CheckedInfo::CheckedInfo(rcstr t): tag(t), cb(nullptr), infoText(nullptr) {
//}

// void CheckedInfo::setText(rcstr text) {
//  EXPECT(infoText)
//  infoText->setText(text);
//}

TableModel::TableModel(gui::TheHub& hub) : RefHub(hub) {}

void TableModel::signalReset() {
    beginResetModel();
    endResetModel();
}
}
