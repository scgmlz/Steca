// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/model_view.h
//! @brief     Defines classes TableModel and TableView
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MODEL_VIEW_H
#define MODEL_VIEW_H

#include <QTreeView>
#include <QAbstractTableModel>


//! The base class of all models of rectangular table form

class TableModel : public QAbstractTableModel {
public:
    //! Redraws the entire table, and sets currentIndex to (0,0) [?] which may be unwanted
    void resetModel() { beginResetModel(); endResetModel(); }

    int columnCount(const QModelIndex& /*unused*/) const { return columnCount(); }
    int rowCount(const QModelIndex& /*unused*/) const { return rowCount(); }

    virtual int columnCount() const = 0;
    virtual int rowCount() const = 0;
};


//! The base class of all views of rectangular table form

//! Based on QTreeView, with hidden 1st column.
//! QTreeView inherits from QAbstractItemView.

class TableView : public QTreeView {
public:
    TableView();
    void setModel(class TableModel* model);
protected:
    virtual class TableModel* model() const;
    int mWidth() const;
};

class CTableView : public TableView {
};

#endif // MODEL_VIEW_H
