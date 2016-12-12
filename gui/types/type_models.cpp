/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "type_models.h"
#include <QCheckBox>
#include <QLineEdit>

namespace models {

QVariant    const EMPTY_VAR;
QModelIndex const ANY_INDEX;

//------------------------------------------------------------------------------

//CheckedInfo::CheckedInfo(rcstr t): tag(t), cb(nullptr), infoText(nullptr) {
//}

//void CheckedInfo::setText(rcstr text) {
//  EXPECT(infoText)
//  infoText->setText(text);
//}

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
