//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/enum_cell.h
//! @brief     Defines class QcrEnumCell
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef ENUM_CELL_H
#define ENUM_CELL_H

#include "qcr/engine/cell.h"
#include <vector>

class QcrEnumCell : public QcrCell<int> {
public:
    QcrEnumCell() : QcrCell<int>{-1} {}
    void setTags(const QStringList&);
    const QStringList& tags() const { return tags_; }

private:
    QStringList tags_;
};

#endif // ENUM_CELL_H
