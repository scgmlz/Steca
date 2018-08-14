//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/enum_cell.cpp
//! @brief     Implements class QcrEnumCell
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "qcr/engine/enum_cell.h"
#include "qcr/widgets/controls.h"

void QcrEnumCell::setTags(const QStringList& _tags)
{
    tags_ = _tags;
    if (value_<0 || tags_.size()<=value_)
        setVal(0);
    if (auto* c = dynamic_cast<QcrComboBox*>(backlink_))
        c->setItems(tags_);
}
