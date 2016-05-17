// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
//
//! @file      type_models.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "type_models.h"
#include <QCheckBox>
#include <QLineEdit>

namespace models {

QVariant    const EMPTY_VAR;
QModelIndex const ANY_INDEX;

//------------------------------------------------------------------------------

CheckedInfo::CheckedInfo(rcstr tag_): tag(tag_), cb(nullptr), infoText(nullptr) {
}

void CheckedInfo::setText(rcstr text) {
  EXPECT(infoText)
  infoText->setText(text);
}

//------------------------------------------------------------------------------

TableModel::TableModel(gui::TheHub& hub): RefHub(hub) {
}

void TableModel::signalReset() {
  beginResetModel();
  endResetModel();
}

//------------------------------------------------------------------------------
}
// eof
