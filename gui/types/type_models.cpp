// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      type_models.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "type_models.h"
#include <QCheckBox>
#include <QLineEdit>

namespace models {

QVariant    const EMPTY_VAR;
QModelIndex const ANY_INDEX;

//------------------------------------------------------------------------------

CheckedInfo::CheckedInfo(rcstr t): tag(t), cb(nullptr), infoText(nullptr) {
}

void CheckedInfo::setText(rcstr text) {
  EXPECT(infoText)
  infoText->setText(text);
}

//------------------------------------------------------------------------------

TableModel::TableModel(gui::TheHub& hub) : RefHub(hub) {
}

void TableModel::signalReset() {
  beginResetModel();
  endResetModel();
}

//------------------------------------------------------------------------------
}
// eof
