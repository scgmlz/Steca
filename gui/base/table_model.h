// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/table_model.h
//! @brief     Defines class TableModel
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TABLE_MODEL_H
#define TABLE_MODEL_H

#include <QAbstractTableModel>

//! The base class of all models of rectangular table form

class TableModel : public QAbstractTableModel {
public:
    void signalReset() { beginResetModel(); endResetModel(); } //!< force-emits reset() signal
    int columnCount(const QModelIndex& /*unused*/) const {
        return columnCount(); }
    int rowCount(const QModelIndex& /*unused*/) const {
        return rowCount(); }
    virtual int columnCount() const = 0;
    virtual int rowCount() const = 0;
};

#endif // TABLE_MODEL_H
